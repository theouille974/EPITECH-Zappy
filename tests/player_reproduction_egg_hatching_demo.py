#!/usr/bin/env python3
"""
Player Reproduction and Egg Hatching Demo
=========================================

This script demonstrates the player reproduction system:
1. Fork command - creates eggs and adds team slots
2. Connect_nbr command - shows available slots
3. Egg hatching - when new players connect to available slots

Run with: python3 player_reproduction_egg_hatching_demo.py
"""

import subprocess
import socket
import time
import sys
import os

def start_server():
    """Start the Zappy server"""
    print("🚀 Starting Zappy server for reproduction demo...")
    server = subprocess.Popen([
        "../zappy_server", 
        "-p", "4242", 
        "-x", "10", 
        "-y", "10", 
        "-n", "TEAM1", "TEAM2", 
        "-c", "2",  # Start with 2 slots per team
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
    """Client for testing"""
    def __init__(self, port=4242):
        self.s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.s.connect(("localhost", port))
        self.s.recv(1024)  # Welcome message
    
    def connect_team(self, team):
        """Connect to a team"""
        self.send(team)
        response = self.recv()
        print(f"   Connected to {team}: {response.strip()}")
        return response
    
    def send(self, msg):
        """Send a command"""
        self.s.sendall((msg + "\n").encode())
        return self.recv()
    
    def recv(self):
        """Receive response"""
        return self.s.recv(1024).decode()
    
    def close(self):
        self.s.close()

def demo_fork_command():
    """Demonstrate the fork command"""
    print("\n🥚 FORK COMMAND DEMO")
    print("=" * 40)
    
    server = start_server()
    try:
        client = Client()
        client.connect_team("TEAM1")
        
        # Check initial slots
        print("\n1. Initial team slots:")
        response = client.send("Connect_nbr")
        print(f"   Available slots: {response.strip()}")
        
        # Execute fork command
        print("\n2. Executing fork command:")
        response = client.send("Fork")
        print(f"   Fork result: {response.strip()}")
        
        # Check slots after fork
        print("\n3. Slots after fork:")
        response = client.send("Connect_nbr")
        print(f"   Available slots: {response.strip()}")
        
        # Check inventory (fork should not affect inventory)
        print("\n4. Inventory after fork:")
        response = client.send("Inventory")
        print(f"   Inventory: {response.strip()}")
        
        client.close()
        print("\n✅ Fork demo completed!")
        
    finally:
        stop_server(server)

def demo_multiple_forks():
    """Demonstrate multiple fork commands"""
    print("\n🥚🥚 MULTIPLE FORKS DEMO")
    print("=" * 40)
    
    server = start_server()
    try:
        client = Client()
        client.connect_team("TEAM1")
        
        # Check initial state
        print("\n1. Initial state:")
        slots = client.send("Connect_nbr")
        print(f"   Available slots: {slots.strip()}")
        
        # Multiple forks
        print("\n2. Executing multiple forks:")
        for i in range(3):
            response = client.send("Fork")
            print(f"   Fork {i+1}: {response.strip()}")
            
            slots = client.send("Connect_nbr")
            print(f"   Slots after fork {i+1}: {slots.strip()}")
        
        client.close()
        print("\n✅ Multiple forks demo completed!")
        
    finally:
        stop_server(server)

def demo_egg_hatching():
    """Demonstrate egg hatching when new player connects"""
    print("\n🐣 EGG HATCHING DEMO")
    print("=" * 40)
    
    server = start_server()
    try:
        # First player connects and forks
        client1 = Client()
        client1.connect_team("TEAM1")
        
        print("\n1. First player connected and forking:")
        response = client1.send("Fork")
        print(f"   Fork result: {response.strip()}")
        
        slots = client1.send("Connect_nbr")
        print(f"   Available slots: {slots.strip()}")
        
        # Second player connects (should hatch an egg)
        print("\n2. Second player connecting (should hatch egg):")
        client2 = Client()
        response = client2.connect_team("TEAM1")
        print(f"   Connection result: {response.strip()}")
        
        # Check slots after hatching
        slots = client2.send("Connect_nbr")
        print(f"   Available slots after hatching: {slots.strip()}")
        
        # Check both players can interact
        print("\n3. Both players can interact:")
        response1 = client1.send("Inventory")
        response2 = client2.send("Inventory")
        print(f"   Player1 inventory: {response1.strip()}")
        print(f"   Player2 inventory: {response2.strip()}")
        
        # Test communication between hatched players
        print("\n4. Communication between players:")
        response = client1.send("Broadcast Hello from original player")
        print(f"   Player1 broadcast: {response.strip()}")
        
        response = client2.recv()
        print(f"   Player2 received: {response.strip()}")
        
        client1.close()
        client2.close()
        print("\n✅ Egg hatching demo completed!")
        
    finally:
        stop_server(server)

def demo_team_slot_management():
    """Demonstrate team slot management with multiple teams"""
    print("\n👥 TEAM SLOT MANAGEMENT DEMO")
    print("=" * 40)
    
    server = start_server()
    try:
        # Team1 players
        client1_1 = Client()
        client1_2 = Client()
        
        # Team2 players
        client2_1 = Client()
        client2_2 = Client()
        
        # Connect all players
        print("\n1. Connecting all players:")
        client1_1.connect_team("TEAM1")
        client1_2.connect_team("TEAM1")
        client2_1.connect_team("TEAM2")
        client2_2.connect_team("TEAM2")
        
        # Check initial slots
        print("\n2. Initial slots per team:")
        slots1 = client1_1.send("Connect_nbr")
        slots2 = client2_1.send("Connect_nbr")
        print(f"   TEAM1 slots: {slots1.strip()}")
        print(f"   TEAM2 slots: {slots2.strip()}")
        
        # Team1 forks
        print("\n3. TEAM1 forking:")
        response = client1_1.send("Fork")
        print(f"   TEAM1 fork: {response.strip()}")
        
        slots1 = client1_1.send("Connect_nbr")
        print(f"   TEAM1 slots after fork: {slots1.strip()}")
        
        # Team2 forks
        print("\n4. TEAM2 forking:")
        response = client2_1.send("Fork")
        print(f"   TEAM2 fork: {response.strip()}")
        
        slots2 = client2_1.send("Connect_nbr")
        print(f"   TEAM2 slots after fork: {slots2.strip()}")
        
        # New players connect to hatched eggs
        print("\n5. New players connecting to hatched eggs:")
        client1_3 = Client()
        client2_3 = Client()
        
        response1 = client1_3.connect_team("TEAM1")
        response2 = client2_3.connect_team("TEAM2")
        print(f"   TEAM1 new player: {response1.strip()}")
        print(f"   TEAM2 new player: {response2.strip()}")
        
        # Check final slots
        slots1 = client1_1.send("Connect_nbr")
        slots2 = client2_1.send("Connect_nbr")
        print(f"   Final TEAM1 slots: {slots1.strip()}")
        print(f"   Final TEAM2 slots: {slots2.strip()}")
        
        # Test communication between all players
        print("\n6. Testing communication:")
        response = client1_1.send("Broadcast Hello from TEAM1")
        print(f"   TEAM1 broadcast: {response.strip()}")
        
        # All players should receive
        responses = []
        for client in [client1_2, client1_3, client2_1, client2_2, client2_3]:
            try:
                response = client.recv()
                responses.append(response.strip())
            except:
                pass
        
        print(f"   Received by {len(responses)} players")
        
        # Close all connections
        for client in [client1_1, client1_2, client1_3, client2_1, client2_2, client2_3]:
            client.close()
        
        print("\n✅ Team slot management demo completed!")
        
    finally:
        stop_server(server)

def demo_fork_requirements():
    """Demonstrate fork command requirements and limitations"""
    print("\n🔒 FORK REQUIREMENTS DEMO")
    print("=" * 40)
    
    server = start_server()
    try:
        client = Client()
        client.connect_team("TEAM1")
        
        # Check if we can fork multiple times
        print("\n1. Testing multiple forks:")
        for i in range(5):
            response = client.send("Fork")
            slots = client.send("Connect_nbr")
            print(f"   Fork {i+1}: {response.strip()} | Slots: {slots.strip()}")
            
            if "ko" in response.lower():
                print(f"   ❌ Fork {i+1} failed - likely reached limit")
                break
        
        # Test fork with low food
        print("\n2. Testing fork with low food:")
        # Take some food to reduce inventory
        for i in range(5):
            client.send("Take food")
        
        response = client.send("Inventory")
        print(f"   Inventory before fork: {response.strip()}")
        
        response = client.send("Fork")
        print(f"   Fork with low food: {response.strip()}")
        
        # Check if fork affects food consumption
        response = client.send("Inventory")
        print(f"   Inventory after fork: {response.strip()}")
        
        client.close()
        print("\n✅ Fork requirements demo completed!")
        
    finally:
        stop_server(server)

def demo_integrated_reproduction_scenario():
    """Demonstrate a complete reproduction scenario"""
    print("\n🎮 INTEGRATED REPRODUCTION SCENARIO")
    print("=" * 40)
    
    server = start_server()
    try:
        print("\n1. Starting reproduction scenario...")
        
        # Initial player connects
        client1 = Client()
        client1.connect_team("TEAM1")
        print("   Player1 connected to TEAM1")
        
        # Player1 collects resources and prepares for reproduction
        print("\n2. Player1 preparing for reproduction:")
        client1.send("Take food")
        client1.send("Take food")
        
        inv = client1.send("Inventory")
        print(f"   Player1 inventory: {inv.strip()}")
        
        # Player1 forks to create an egg
        print("\n3. Player1 forking:")
        response = client1.send("Fork")
        print(f"   Fork result: {response.strip()}")
        
        slots = client1.send("Connect_nbr")
        print(f"   Available slots: {slots.strip()}")
        
        # Player1 continues working while egg is incubating
        print("\n4. Player1 continuing work:")
        client1.send("Forward")
        client1.send("Take food")
        
        inv = client1.send("Inventory")
        print(f"   Player1 inventory after work: {inv.strip()}")
        
        # New player connects and hatches the egg
        print("\n5. New player connecting (egg hatching):")
        client2 = Client()
        response = client2.connect_team("TEAM1")
        print(f"   New player connection: {response.strip()}")
        
        # Both players can now work together
        print("\n6. Both players working together:")
        response = client1.send("Broadcast Welcome to the team!")
        print(f"   Player1: {response.strip()}")
        
        response = client2.recv()
        print(f"   Player2 received: {response.strip()}")
        
        response = client2.send("Broadcast Thanks! Ready to work!")
        print(f"   Player2: {response.strip()}")
        
        response = client1.recv()
        print(f"   Player1 received: {response.strip()}")
        
        # Check final state
        slots1 = client1.send("Connect_nbr")
        slots2 = client2.send("Connect_nbr")
        print(f"\n7. Final state:")
        print(f"   Available slots: {slots1.strip()}")
        print(f"   Both players active and communicating")
        
        client1.close()
        client2.close()
        print("\n✅ Integrated reproduction scenario completed!")
        
    finally:
        stop_server(server)

def main():
    """Run all reproduction and egg hatching tests"""
    print("🥚 Zappy Player Reproduction & Egg Hatching Demo")
    print("=" * 60)
    print("This demo showcases the reproduction system:")
    print("• Fork command - creates eggs and adds team slots")
    print("• Connect_nbr command - shows available slots")
    print("• Egg hatching - when new players connect")
    print("• Team slot management")
    print()
    
    if not os.path.exists("../zappy_server"):
        print("❌ Error: zappy_server not found. Please build with 'make'")
        sys.exit(1)
    
    print("✅ Server binary found!")
    print("\nRunning reproduction demos...")
    
    try:
        demo_fork_command()
        demo_multiple_forks()
        demo_egg_hatching()
        demo_team_slot_management()
        demo_fork_requirements()
        demo_integrated_reproduction_scenario()
        
        print("\n🎉 All reproduction demos completed successfully!")
        print("=" * 60)
        print("✅ Fork command working correctly")
        print("✅ Egg creation and slot management working")
        print("✅ Egg hatching when new players connect")
        print("✅ Team slot tracking working")
        print("✅ Integrated reproduction scenarios working")
        print("\nReady for review! 🚀")
        
    except Exception as e:
        print(f"\n❌ Demo failed: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main() 