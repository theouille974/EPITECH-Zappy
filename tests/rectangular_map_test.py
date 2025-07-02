#!/usr/bin/env python3
"""
Rectangular Map Test for Zappy
Tests the server with a rectangular map (20x10) instead of square dimensions.
This tests how the server handles non-square world dimensions.
"""

import subprocess
import socket
import time
import threading
import sys
import os
import signal

def start_server():
    """Start the Zappy server with rectangular map (20x10)"""
    print("🚀 Starting Zappy server with rectangular map (20x10)...")
    server = subprocess.Popen([
        "../zappy_server", 
        "-p", "4242",  # Standard port
        "-x", "20",    # Width: 20
        "-y", "10",    # Height: 10 (rectangular!)
        "-n", "RED", "BLUE", "GREEN", 
        "-c", "15",    # 5 players per team
        "-f", "10"
    ], cwd=".", stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    
    # Wait for server to start
    time.sleep(3)
    return server

def stop_server(server):
    """Stop the server gracefully"""
    print("🛑 Stopping server...")
    server.terminate()
    try:
        server.wait(timeout=10)
    except subprocess.TimeoutExpired:
        server.kill()
        server.wait()

def start_ai(team_name, ai_id):
    """Start a single AI process"""
    try:
        ai = subprocess.Popen([
            "../zappy_ia",
            "-p", "4242",  # Standard port
            "-n", team_name,
            "-h", "localhost",
            "--quiet"
        ], cwd=".", stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        
        print(f"🤖 AI {ai_id} ({team_name}): Started")
        return ai
        
    except Exception as e:
        print(f"❌ AI {ai_id} ({team_name}): Failed to start - {e}")
        return None

def connect_team_ais(team_name, count):
    """Start multiple AIs for a team"""
    ais = []
    
    print(f"🤖 Starting {count} {team_name} AIs...")
    
    for i in range(count):
        ai = start_ai(team_name, i + 1)
        if ai:
            ais.append(ai)
        else:
            print(f"❌ Failed to start {team_name} AI {i + 1}")
        
        # Small delay between starts
        time.sleep(0.3)
    
    print(f"✅ Successfully started {len(ais)} {team_name} AIs")
    return ais

def test_rectangular_connection():
    """Test basic connection to rectangular map"""
    print("🔍 Testing connection to rectangular map...")
    
    try:
        # Create socket connection
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect(("localhost", 4242))
        
        # Get welcome message
        welcome = s.recv(1024).decode()
        print(f"📨 Welcome message: {welcome.strip()}")
        
        # Test team connection
        s.sendall(b"RED\n")
        response = s.recv(1024).decode()
        print(f"📨 Team response: {response.strip()}")
        
        # Should contain "4\n20 10" (4 remaining slots, 20x10 map)
        if "20 10" in response:
            print("✅ Rectangular map dimensions confirmed!")
        else:
            print("❌ Expected rectangular dimensions not found")
        
        s.close()
        return True
        
    except Exception as e:
        print(f"❌ Connection test failed: {e}")
        return False

def signal_handler(signum, frame):
    """Handle Ctrl+C gracefully"""
    print("\n🛑 Received interrupt signal. Shutting down...")
    sys.exit(0)

def main():
    """Main test function"""
    print("=== Zappy Rectangular Map Test ===")
    print("Server: 20x10 rectangular board, RED/BLUE/GREEN teams")
    print("Map: 20 tiles wide, 10 tiles tall (rectangular!)")
    print("Teams: 3 teams with 5 players each (15 total)")
    print("GUI: Launch manually with: ./zappy_gui -p 4242 -h localhost")
    print()
    
    # Set up signal handler for graceful shutdown
    signal.signal(signal.SIGINT, signal_handler)
    
    # Check if binaries exist
    if not os.path.exists("../zappy_server"):
        print("❌ Error: zappy_server binary not found")
        print("Please build the server first")
        sys.exit(1)
        
    if not os.path.exists("../zappy_ia"):
        print("❌ Error: zappy_ia binary not found")
        print("Please build the AI client first")
        sys.exit(1)
    
    server = None
    red_ais = []
    blue_ais = []
    green_ais = []
    
    try:
        # Start server
        server = start_server()
        print("✅ Server started successfully")
        
        # Test connection to rectangular map
        if not test_rectangular_connection():
            print("❌ Connection test failed, stopping test")
            return
        
        # Start RED team AIs
        red_ais = connect_team_ais("RED", 5)
        
        # Start BLUE team AIs  
        blue_ais = connect_team_ais("BLUE", 5)
        
        # Start GREEN team AIs
        green_ais = connect_team_ais("GREEN", 5)
        
        total_started = len(red_ais) + len(blue_ais) + len(green_ais)
        print(f"\n📊 Total AIs started: {total_started}/15")
        
        if total_started == 15:
            print("🎉 SUCCESS: All 15 AIs started on rectangular map!")
        else:
            print(f"⚠️  WARNING: Only {total_started}/15 AIs started")
        
        print("\n🎮 Rectangular map test is now running!")
        print("• Server: 20x10 rectangular world")
        print("• Teams: RED, BLUE, GREEN (5 players each)")
        print("• Map: 20 tiles wide × 10 tiles tall")
        print("• Duration: Run as long as you want")
        print("\n💡 To watch the action:")
        print("  • Open a new terminal")
        print("  • Run: ./zappy_gui -p 4242 -h localhost")
        print("  • Observe how AIs navigate the rectangular world!")
        print("\n⏰ Test started at:", time.strftime("%H:%M:%S"))
        print("🔄 Running indefinitely...")
        print("🛑 Press Ctrl+C to stop")
        
        # Keep running indefinitely
        while True:
            time.sleep(1)
            
    except KeyboardInterrupt:
        print("\n🛑 Test interrupted by user")
    except Exception as e:
        print(f"❌ Test failed: {e}")
    finally:
        # Cleanup
        print("🧹 Cleaning up...")
        
        # Stop all AIs
        print("🤖 Stopping AIs...")
        for ai in red_ais + blue_ais + green_ais:
            if ai:
                ai.terminate()
                try:
                    ai.wait(timeout=5)
                except subprocess.TimeoutExpired:
                    ai.kill()
        
        # Stop server
        if server:
            stop_server(server)
        
        print("✅ Cleanup completed!")
        print("⏰ Test ended at:", time.strftime("%H:%M:%S"))

if __name__ == "__main__":
    main() 