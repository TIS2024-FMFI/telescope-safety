package main

import (
	"bufio"
	"encoding/json"
	"fmt"
	"github.com/gorilla/websocket"
	"net"
	"net/http"
	"sync"
	"time"
)

type Data struct {
	id        string
	time      time.Time
	azimuth   int
	elevation int
}

type Client struct {
	conn *websocket.Conn
	send chan []byte
}

var (
	clients   = make(map[*Client]bool) // Track connected clients
	broadcast = make(chan []byte)      // Channel to broadcast messages
	mutex     sync.Mutex               // Mutex to protect clients map
)

var upgrader = websocket.Upgrader{
	CheckOrigin: func(r *http.Request) bool { return true },
}

func unwrap(r []byte, data *Data) error {
	err := json.Unmarshal(r, &data)
	if err != nil {
		return err
	}
	return nil
}

func handler(conn net.Conn) {
	reader := bufio.NewReader(conn)

	for {
		r, err := reader.ReadBytes('\n')
		if err != nil {
			fmt.Println("Error reading from connection")
			break
		}
		//fmt.Printf("Reeceived message: %d\n", r)

		//Could be skipped
		//var data Data
		//err = unwrap(r, &data)
		//if err != nil {
		//	break
		//}

		// Send data to webserver
		//mutex.Lock()
		if len(clients) > 0 {
			broadcast <- r
		}
		//mutex.Unlock()

	}
}

func reciveData() {
	listener, err := net.Listen("tcp", ":9001")
	if err != nil {
		fmt.Println("Error listening localhost:9001", err)
		panic(err)
	}
	defer listener.Close()

	for {
		conn, err := listener.Accept()
		if err != nil {
			panic(err)
		}
		go handler(conn)

	}
}

// Handle incoming WebSocket connections
func handleConnections(w http.ResponseWriter, r *http.Request) {
	// Upgrade HTTP to WebSocket
	fmt.Println("Upgrading connection")
	conn, err := upgrader.Upgrade(w, r, nil)
	if err != nil {
		fmt.Println("Error upgrading connection:", err)
		return
	}
	defer conn.Close()
	fmt.Println("New client connected")

	client := &Client{conn: conn, send: make(chan []byte)}
	// Register new client
	mutex.Lock()
	clients[client] = true
	mutex.Unlock()

	fmt.Println("New client registered")

	// Clean up when the client disconnects
	defer func() {
		close(client.send)
		mutex.Lock()
		delete(clients, client)
		mutex.Unlock()
		fmt.Println("Client disconnected")
	}()

	for msg := range client.send {
		err = conn.WriteMessage(websocket.TextMessage, msg)
		if err != nil {
			fmt.Println("Error writing to client:", err)
			return
		}
	}

}

// Broadcast messages to all connected clients
func handleBroadcast() {
	for {
		// Get next message to broadcast
		message := <-broadcast
		fmt.Print("sending message: ", string(message))
		fmt.Println(len(clients))

		// Send message to each client
		mutex.Lock()
		for client := range clients {
			select {
			case client.send <- message:
			default:
				close(client.send)
				delete(clients, client)
			}
		}
		mutex.Unlock()
	}
}

func startHttpServer() {
	server := http.Server{}
	server.Addr = ":80"
	server.Handler = http.FileServer(http.Dir("./www"))
	err := server.ListenAndServe()
	if err != nil {
		fmt.Println("Error starting server localhost:8080", err)
		return
	}
	fmt.Println("Server started")
}

func startWSServer() {

	http.HandleFunc("/", handleConnections)
	err := http.ListenAndServe(":8080", nil)
	if err != nil {
		fmt.Println("Error starting server localhost:8080")
	}
	fmt.Println("Listening on port 8080")
}

func main() {

	go reciveData()
	go startHttpServer()

	go startWSServer()
	go handleBroadcast()

	for {
	}

}
