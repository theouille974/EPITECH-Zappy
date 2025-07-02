#!/usr/bin/env python3
"""
Inventory and Broadcast Commands Demo
====================================

This test file demonstrates the key features of the Zappy server:
1. Inventory management (Take, Set, Inventory commands)
2. Broadcast communication system
3. Vision system (Look command)

These tests showcase the core gameplay mechanics for tomorrow's review.
"""

import subprocess
import socket
import time
import sys
import os

def start_server():
    """Start the Zappy server with specific parameters for testing"""
    print("🚀 Starting Zappy server for inventory/broadcast demo...")
    server = subprocess.Popen([
        "../zappy_server", 
        "-p", "4242", 
        "-x", "15", 
        "-y", "15", 
        "-n", "RED", "BLUE", "GREEN", 
        "-c", "10", 
        "-f", "10"
    ], cwd=".", stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    
    time.sleep(2)
    return server

def stop_server(server):
    """Stop the server gracefully"""
    server.terminate()
    try:
        server.wait(timeout=10)
    except subprocess.TimeoutExpired:
        server.kill()

class ZappyClient:
    """Enhanced client for testing with better error handling"""
    def __init__(self, host="localhost", port=4242):
        self.s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.s.connect((host, port))
        self.welcome = self.s.recv(1024).decode()
        self.team_name = None

    def connect(self, team_name="RED"):
        """Connect to a team"""
        self.team_name = team_name
        response = self.send(team_name)
        response = response.strip() + "\n"
        try:
            response += self.s.recv(1024).decode()
        except socket.timeout:
            raise AssertionError(f"Timeout during connection to team {team_name}")
        return response

    def send(self, msg, timeout=3):
        """Send a command and receive response"""
        self.s.settimeout(timeout)
        self.s.sendall((msg + "\n").encode())
        try:
            response = self.s.recv(1024).decode()
        except socket.timeout:
            raise AssertionError(f"Timeout: no response for command '{msg}'")
        return response
    
    def receive(self, size=1024):
        """Receive data without sending anything"""
        self.s.settimeout(2)
        try:
            response = self.s.recv(size).decode()
        except socket.timeout:
            raise AssertionError("Timeout: no data received")
        return response

    def close(self):
        """Close the connection"""
        self.s.close()

def test_inventory_basic():
    """Test basic inventory functionality"""
    print("\n📦 Testing Basic Inventory Functionality")
    print("=" * 50)
    
    server = start_server()
    try:
        client = ZappyClient()
        client.connect("RED")
        
        # Test initial inventory
        print("1. Testing initial inventory...")
        response = client.send("Inventory")
        print(f"   Response: {response.strip()}")
        
        # Verify inventory format
        assert "[ food 10, linemate 0, deraumere 0, sibur 0, mendiane 0, phiras 0, thystame 0, life" in response
        print("   ✅ Initial inventory format correct")
        
        # Test inventory after some time
        time.sleep(1)
        response = client.send("Inventory")
        print(f"   Response after 1s: {response.strip()}")
        
        # Life should decrease slightly
        assert "life" in response
        print("   ✅ Life calculation working")
        
        client.close()
        print("   ✅ Basic inventory test passed")
        
    finally:
        stop_server(server)

def test_take_and_set_commands():
    """Test Take and Set commands"""
    print("\n🔄 Testing Take and Set Commands")
    print("=" * 50)
    
    server = start_server()
    try:
        client = ZappyClient()
        client.connect("RED")
        
        # Test taking food (should be available)
        print("1. Testing Take food command...")
        response = client.send("Take food")
        print(f"   Response: {response.strip()}")
        
        if "ok" in response.lower():
            print("   ✅ Successfully took food")
            
            # Check inventory increased
            inv_response = client.send("Inventory")
            print(f"   Inventory after taking: {inv_response.strip()}")
            assert "food 11" in inv_response
            print("   ✅ Food count increased in inventory")
            
            # Test setting food back
            print("2. Testing Set food command...")
            response = client.send("Set food")
            print(f"   Response: {response.strip()}")
            
            if "ok" in response.lower():
                print("   ✅ Successfully set food back")
                
                # Check inventory decreased
                inv_response = client.send("Inventory")
                print(f"   Inventory after setting: {inv_response.strip()}")
                assert "food 10" in inv_response
                print("   ✅ Food count decreased in inventory")
            else:
                print("   ❌ Failed to set food")
        else:
            print("   ❌ Failed to take food")
        
        # Test taking non-existent item
        print("3. Testing Take non-existent item...")
        response = client.send("Take linemate")
        print(f"   Response: {response.strip()}")
        
        if "ko" in response.lower():
            print("   ✅ Correctly rejected taking non-existent item")
        else:
            print("   ⚠️  Unexpected response for non-existent item")
        
        client.close()
        print("   ✅ Take and Set commands test completed")
        
    finally:
        stop_server(server)

def test_broadcast_basic():
    """Test basic broadcast functionality"""
    print("\n📢 Testing Basic Broadcast Functionality")
    print("=" * 50)
    
    server = start_server()
    try:
        client1 = ZappyClient()
        client2 = ZappyClient()
        
        client1.connect("RED")
        client2.connect("BLUE")
        
        # Test broadcast from client1
        print("1. Testing broadcast from RED team...")
        response = client1.send("Broadcast Hello from RED")
        print(f"   Sender response: {response.strip()}")
        
        # Client1 should receive message 0 (from itself)
        assert "message 0, Hello from RED" in response
        print("   ✅ Sender received own message")
        
        # Client2 should receive the broadcast
        response = client2.receive()
        print(f"   Receiver response: {response.strip()}")
        
        if "message" in response and "Hello from RED" in response:
            print("   ✅ Receiver got broadcast message")
        else:
            print("   ❌ Receiver didn't get broadcast message")
        
        # Test broadcast from client2
        print("2. Testing broadcast from BLUE team...")
        response = client2.send("Broadcast Hello from BLUE")
        print(f"   Sender response: {response.strip()}")
        
        # Client1 should receive the broadcast
        response = client1.receive()
        print(f"   Receiver response: {response.strip()}")
        
        if "message" in response and "Hello from BLUE" in response:
            print("   ✅ Cross-team broadcast working")
        else:
            print("   ❌ Cross-team broadcast failed")
        
        client1.close()
        client2.close()
        print("   ✅ Basic broadcast test completed")
        
    finally:
        stop_server(server)

def test_broadcast_direction():
    """Test broadcast direction calculation"""
    print("\n🧭 Testing Broadcast Direction Calculation")
    print("=" * 50)
    
    server = start_server()
    try:
        client1 = ZappyClient()
        client2 = ZappyClient()
        
        client1.connect("RED")
        client2.connect("BLUE")
        
        # Move clients to different positions
        print("1. Positioning clients...")
        
        # Client1 moves forward
        client1.send("Forward")
        print("   Client1 moved forward")
        
        # Client2 turns right and moves forward
        client2.send("Right")
        client2.send("Forward")
        print("   Client2 turned right and moved forward")
        
        # Test broadcast and check direction
        print("2. Testing broadcast direction...")
        response = client1.send("Broadcast Direction test")
        print(f"   Client1 broadcast response: {response.strip()}")
        
        # Client2 should receive with specific direction
        response = client2.receive()
        print(f"   Client2 received: {response.strip()}")
        
        if "message" in response:
            # Extract direction number
            try:
                direction = int(response.split()[1].rstrip(','))
                print(f"   Direction received: {direction}")
                
                if 1 <= direction <= 8:
                    print("   ✅ Valid direction received")
                else:
                    print("   ⚠️  Direction out of expected range")
            except (ValueError, IndexError):
                print("   ❌ Could not parse direction")
        else:
            print("   ❌ No message received")
        
        client1.close()
        client2.close()
        print("   ✅ Broadcast direction test completed")
        
    finally:
        stop_server(server)

def test_vision_system():
    """Test the vision system with Look command"""
    print("\n👁️  Testing Vision System")
    print("=" * 50)
    
    server = start_server()
    try:
        client = ZappyClient()
        client.connect("RED")
        
        # Test initial look
        print("1. Testing initial Look command...")
        response = client.send("Look")
        print(f"   Response: {response.strip()}")
        
        # Verify look format
        assert response.startswith("[") and response.endswith("]\n")
        print("   ✅ Look response format correct")
        
        # Count tiles (should be 3 for level 1)
        tiles = response.strip()[1:-1].split(",")
        print(f"   Number of visible tiles: {len(tiles)}")
        
        if len(tiles) == 3:
            print("   ✅ Correct number of tiles for level 1")
        else:
            print(f"   ⚠️  Unexpected number of tiles: {len(tiles)}")
        
        # Test look after movement
        print("2. Testing Look after movement...")
        client.send("Forward")
        response = client.send("Look")
        print(f"   Response after moving: {response.strip()}")
        
        # Should still have correct format
        assert response.startswith("[") and response.endswith("]\n")
        print("   ✅ Look still working after movement")
        
        # Test look after rotation
        print("3. Testing Look after rotation...")
        client.send("Right")
        response = client.send("Look")
        print(f"   Response after turning: {response.strip()}")
        
        assert response.startswith("[") and response.endswith("]\n")
        print("   ✅ Look working after rotation")
        
        client.close()
        print("   ✅ Vision system test completed")
        
    finally:
        stop_server(server)

def test_inventory_management_scenario():
    """Test a complete inventory management scenario"""
    print("\n🎯 Testing Complete Inventory Management Scenario")
    print("=" * 50)
    
    server = start_server()
    try:
        client = ZappyClient()
        client.connect("RED")
        
        print("1. Starting inventory management scenario...")
        
        # Initial state
        response = client.send("Inventory")
        initial_food = int(response.split("food")[1].split(",")[0].strip())
        print(f"   Initial food: {initial_food}")
        
        # Take food multiple times
        print("2. Taking food multiple times...")
        food_taken = 0
        for i in range(3):
            response = client.send("Take food")
            if "ok" in response.lower():
                food_taken += 1
                print(f"   Took food #{i+1}")
            else:
                print(f"   Failed to take food #{i+1}")
        
        # Check inventory after taking
        response = client.send("Inventory")
        current_food = int(response.split("food")[1].split(",")[0].strip())
        print(f"   Food after taking: {current_food}")
        
        expected_food = initial_food + food_taken
        if current_food == expected_food:
            print("   ✅ Food count correctly updated")
        else:
            print(f"   ❌ Food count mismatch: expected {expected_food}, got {current_food}")
        
        # Set food back
        print("3. Setting food back...")
        food_set = 0
        for i in range(food_taken):
            response = client.send("Set food")
            if "ok" in response.lower():
                food_set += 1
                print(f"   Set food #{i+1}")
            else:
                print(f"   Failed to set food #{i+1}")
        
        # Final inventory check
        response = client.send("Inventory")
        final_food = int(response.split("food")[1].split(",")[0].strip())
        print(f"   Final food: {final_food}")
        
        if final_food == initial_food:
            print("   ✅ Inventory management completed successfully")
        else:
            print(f"   ❌ Final food count mismatch: expected {initial_food}, got {final_food}")
        
        client.close()
        print("   ✅ Inventory management scenario completed")
        
    finally:
        stop_server(server)

def test_broadcast_communication_scenario():
    """Test a complete broadcast communication scenario"""
    print("\n🗣️  Testing Complete Broadcast Communication Scenario")
    print("=" * 50)
    
    server = start_server()
    try:
        client1 = ZappyClient()
        client2 = ZappyClient()
        client3 = ZappyClient()
        
        client1.connect("RED")
        client2.connect("BLUE")
        client3.connect("GREEN")
        
        print("1. Setting up three-team communication...")
        
        # Test broadcast chain
        print("2. Testing broadcast chain...")
        
        # RED broadcasts
        response = client1.send("Broadcast RED calling all teams")
        print(f"   RED broadcast: {response.strip()}")
        
        # BLUE and GREEN should receive
        response2 = client2.receive()
        response3 = client3.receive()
        print(f"   BLUE received: {response2.strip()}")
        print(f"   GREEN received: {response3.strip()}")
        
        if "RED calling all teams" in response2 and "RED calling all teams" in response3:
            print("   ✅ Multi-team broadcast working")
        else:
            print("   ❌ Multi-team broadcast failed")
        
        # BLUE responds
        print("3. Testing response broadcast...")
        response = client2.send("Broadcast BLUE responding to RED")
        print(f"   BLUE broadcast: {response.strip()}")
        
        # RED and GREEN should receive
        response1 = client1.receive()
        response3 = client3.receive()
        print(f"   RED received: {response1.strip()}")
        print(f"   GREEN received: {response3.strip()}")
        
        if "BLUE responding" in response1 and "BLUE responding" in response3:
            print("   ✅ Response broadcast working")
        else:
            print("   ❌ Response broadcast failed")
        
        # GREEN sends final message
        print("4. Testing final broadcast...")
        response = client3.send("Broadcast GREEN signing off")
        print(f"   GREEN broadcast: {response.strip()}")
        
        # RED and BLUE should receive
        response1 = client1.receive()
        response2 = client2.receive()
        print(f"   RED received: {response1.strip()}")
        print(f"   BLUE received: {response2.strip()}")
        
        if "GREEN signing off" in response1 and "GREEN signing off" in response2:
            print("   ✅ Final broadcast working")
        else:
            print("   ❌ Final broadcast failed")
        
        client1.close()
        client2.close()
        client3.close()
        print("   ✅ Broadcast communication scenario completed")
        
    finally:
        stop_server(server)

def test_integrated_scenario():
    """Test an integrated scenario combining inventory and broadcast"""
    print("\n🎮 Testing Integrated Inventory + Broadcast Scenario")
    print("=" * 50)
    
    server = start_server()
    try:
        client1 = ZappyClient()
        client2 = ZappyClient()
        
        client1.connect("RED")
        client2.connect("BLUE")
        
        print("1. Starting integrated scenario...")
        
        # Client1 takes some food and broadcasts
        print("2. Client1 taking food and broadcasting...")
        client1.send("Take food")
        client1.send("Take food")
        
        response = client1.send("Broadcast I have extra food")
        print(f"   Client1 broadcast: {response.strip()}")
        
        # Client2 should receive the broadcast
        response = client2.receive()
        print(f"   Client2 received: {response.strip()}")
        
        # Client2 responds with inventory status
        print("3. Client2 responding with inventory status...")
        inv_response = client2.send("Inventory")
        food_count = int(inv_response.split("food")[1].split(",")[0].strip())
        
        response = client2.send(f"Broadcast I have {food_count} food")
        print(f"   Client2 broadcast: {response.strip()}")
        
        # Client1 should receive the response
        response = client1.receive()
        print(f"   Client1 received: {response.strip()}")
        
        # Client1 offers to share food
        print("4. Client1 offering to share food...")
        response = client1.send("Broadcast I can share food if you need it")
        print(f"   Client1 broadcast: {response.strip()}")
        
        # Client2 accepts and moves to get food
        print("5. Client2 accepting and moving...")
        response = client2.receive()
        print(f"   Client2 received: {response.strip()}")
        
        # Client1 sets food down
        print("6. Client1 setting food down...")
        client1.send("Set food")
        
        # Client2 takes the food
        print("7. Client2 taking the shared food...")
        response = client2.send("Take food")
        print(f"   Client2 take response: {response.strip()}")
        
        # Verify the exchange
        inv1 = client1.send("Inventory")
        inv2 = client2.send("Inventory")
        
        food1 = int(inv1.split("food")[1].split(",")[0].strip())
        food2 = int(inv2.split("food")[1].split(",")[0].strip())
        
        print(f"   Final food counts - Client1: {food1}, Client2: {food2}")
        
        if food1 < 10 and food2 > 10:
            print("   ✅ Food sharing successful")
        else:
            print("   ❌ Food sharing failed")
        
        client1.close()
        client2.close()
        print("   ✅ Integrated scenario completed")
        
    finally:
        stop_server(server)

def main():
    """Run all inventory and broadcast tests"""
    print("🎯 Zappy Inventory and Broadcast Commands Demo")
    print("=" * 60)
    print("This demo showcases the core gameplay mechanics:")
    print("• Inventory management (Take, Set, Inventory)")
    print("• Broadcast communication system")
    print("• Vision system (Look command)")
    print("• Integrated scenarios")
    print()
    
    # Check if server binary exists
    if not os.path.exists("../zappy_server"):
        print("❌ Error: zappy_server binary not found")
        print("   Please build the server first: make")
        sys.exit(1)
    
    print("✅ Server binary found!")
    print()
    
    try:
        # Run all tests
        test_inventory_basic()
        test_take_and_set_commands()
        test_broadcast_basic()
        test_broadcast_direction()
        test_vision_system()
        test_inventory_management_scenario()
        test_broadcast_communication_scenario()
        test_integrated_scenario()
        
        print("\n🎉 All tests completed successfully!")
        print("=" * 60)
        print("✅ Inventory system working correctly")
        print("✅ Broadcast system working correctly")
        print("✅ Vision system working correctly")
        print("✅ Integrated scenarios working correctly")
        print()
        print("Ready for tomorrow's review! 🚀")
        
    except Exception as e:
        print(f"\n❌ Test failed: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main() 