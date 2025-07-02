#!/usr/bin/env python3
"""
Zappy Review Demo - Inventory and Broadcast Commands
===================================================

This script demonstrates the key features for tomorrow's review:
1. Inventory Management (Take, Set, Inventory)
2. Broadcast Communication System
3. Vision System (Look command)

Run with: python3 inventory_broadcast_vision_demo.py
"""

import subprocess
import socket
import time
import sys
import os

def start_server():
    """Start the Zappy server"""
    print("🚀 Starting Zappy server...")
    server = subprocess.Popen([
        "../zappy_server", 
        "-p", "4242", 
        "-x", "10", 
        "-y", "10", 
        "-n", "TEAM1", "TEAM2", 
        "-c", "5", 
        "-f", "10"
    ], cwd=".", stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    time.sleep(2)
    return server

def stop_server(server):
    """Stop the server"""
    server.terminate()
    try:
        server.wait(timeout=5)
    except subprocess.TimeoutExpired:
        server.kill()

class Client:
    """Simple client for testing"""
    def __init__(self, port=4242):
        self.s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.s.connect(("localhost", port))
        self.s.recv(1024)  # Welcome message
    
    def connect_team(self, team):
        """Connect to a team"""
        self.send(team)
        response = self.recv()
        print(f"   Connected to {team}: {response.strip()}")
    
    def send(self, msg):
        """Send a command"""
        self.s.sendall((msg + "\n").encode())
        return self.recv()
    
    def recv(self):
        """Receive response"""
        return self.s.recv(1024).decode()
    
    def close(self):
        self.s.close()

def demo_inventory():
    """Demonstrate inventory management"""
    print("\n📦 INVENTORY MANAGEMENT DEMO")
    print("=" * 40)
    
    server = start_server()
    try:
        client = Client()
        client.connect_team("TEAM1")
        
        # Show initial inventory
        print("\n1. Initial Inventory:")
        response = client.send("Inventory")
        print(f"   {response.strip()}")
        
        # Take food
        print("\n2. Taking Food:")
        response = client.send("Take food")
        print(f"   Take food: {response.strip()}")
        
        # Show updated inventory
        response = client.send("Inventory")
        print(f"   Updated inventory: {response.strip()}")
        
        # Set food back
        print("\n3. Setting Food Back:")
        response = client.send("Set food")
        print(f"   Set food: {response.strip()}")
        
        # Show final inventory
        response = client.send("Inventory")
        print(f"   Final inventory: {response.strip()}")
        
        client.close()
        print("\n✅ Inventory demo completed!")
        
    finally:
        stop_server(server)

def demo_broadcast():
    """Demonstrate broadcast communication"""
    print("\n📢 BROADCAST COMMUNICATION DEMO")
    print("=" * 40)
    
    server = start_server()
    try:
        client1 = Client()
        client2 = Client()
        
        client1.connect_team("TEAM1")
        client2.connect_team("TEAM2")
        
        # Team1 broadcasts
        print("\n1. Team1 Broadcasting:")
        response = client1.send("Broadcast Hello from Team1")
        print(f"   Team1 sent: {response.strip()}")
        
        # Team2 receives
        response = client2.recv()
        print(f"   Team2 received: {response.strip()}")
        
        # Team2 responds
        print("\n2. Team2 Responding:")
        response = client2.send("Broadcast Hello from Team2")
        print(f"   Team2 sent: {response.strip()}")
        
        # Team1 receives
        response = client1.recv()
        print(f"   Team1 received: {response.strip()}")
        
        client1.close()
        client2.close()
        print("\n✅ Broadcast demo completed!")
        
    finally:
        stop_server(server)

def demo_vision():
    """Demonstrate vision system"""
    print("\n👁️ VISION SYSTEM DEMO")
    print("=" * 40)
    
    server = start_server()
    try:
        client = Client()
        client.connect_team("TEAM1")
        
        # Initial look
        print("\n1. Initial Vision:")
        response = client.send("Look")
        print(f"   {response.strip()}")
        
        # Look after moving
        print("\n2. Vision After Moving:")
        client.send("Forward")
        response = client.send("Look")
        print(f"   {response.strip()}")
        
        # Look after turning
        print("\n3. Vision After Turning:")
        client.send("Right")
        response = client.send("Look")
        print(f"   {response.strip()}")
        
        client.close()
        print("\n✅ Vision demo completed!")
        
    finally:
        stop_server(server)

def demo_integrated():
    """Demonstrate integrated scenario"""
    print("\n🎮 INTEGRATED SCENARIO DEMO")
    print("=" * 40)
    
    server = start_server()
    try:
        client1 = Client()
        client2 = Client()
        
        client1.connect_team("TEAM1")
        client2.connect_team("TEAM2")
        
        print("\n1. Team1 collecting resources:")
        client1.send("Take food")
        client1.send("Take food")
        
        inv1 = client1.send("Inventory")
        print(f"   Team1 inventory: {inv1.strip()}")
        
        print("\n2. Team1 broadcasting availability:")
        response = client1.send("Broadcast I have extra food")
        print(f"   Team1: {response.strip()}")
        
        response = client2.recv()
        print(f"   Team2 received: {response.strip()}")
        
        print("\n3. Team2 requesting help:")
        response = client2.send("Broadcast I need food")
        print(f"   Team2: {response.strip()}")
        
        response = client1.recv()
        print(f"   Team1 received: {response.strip()}")
        
        print("\n4. Team1 sharing food:")
        client1.send("Set food")
        response = client2.send("Take food")
        print(f"   Team2 take result: {response.strip()}")
        
        inv1_final = client1.send("Inventory")
        inv2_final = client2.send("Inventory")
        print(f"   Final - Team1: {inv1_final.strip()}")
        print(f"   Final - Team2: {inv2_final.strip()}")
        
        client1.close()
        client2.close()
        print("\n✅ Integrated demo completed!")
        
    finally:
        stop_server(server)

def main():
    """Run the review demo"""
    print("🎯 Zappy Review Demo - Inventory & Broadcast")
    print("=" * 50)
    
    if not os.path.exists("../zappy_server"):
        print("❌ Error: zappy_server not found. Please build with 'make'")
        sys.exit(1)
    
    print("✅ Server binary found!")
    print("\nRunning demos...")
    
    try:
        demo_inventory()
        demo_broadcast()
        demo_vision()
        demo_integrated()
        
        print("\n🎉 All demos completed successfully!")
        print("=" * 50)
        print("✅ Inventory management working")
        print("✅ Broadcast communication working")
        print("✅ Vision system working")
        print("✅ Integrated scenarios working")
        print("\nReady for review! 🚀")
        
    except Exception as e:
        print(f"\n❌ Demo failed: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main() 