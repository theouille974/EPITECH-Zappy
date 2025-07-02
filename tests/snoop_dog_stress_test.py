#!/usr/bin/env python3
"""
Snoop Dog Stress Test for Zappy
Tests the server with ONLY SNOOP_DOG team and many AIs.
This creates a massive Snoop Dog party with green faces everywhere!
"""

import subprocess
import socket
import time
import threading
import sys
import os
import signal

def start_server():
    """Start the Zappy server with ONLY SNOOP_DOG team"""
    print("🚀 Starting Zappy server with SNOOP_DOG team only...")
    server = subprocess.Popen([
        "../zappy_server", 
        "-p", "4242",  # Standard port
        "-x", "30",    # Large map for many Snoops
        "-y", "30", 
        "-n", "SNOOP_DOG",  # ONLY SNOOP_DOG team!
        "-c", "50",    # 50 Snoop Dogs maximum
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

def start_ai(ai_id):
    """Start a single SNOOP_DOG AI process"""
    try:
        ai = subprocess.Popen([
            "../zappy_ia",
            "-p", "4242",  # Standard port
            "-n", "SNOOP_DOG",
            "-h", "localhost",
            "--quiet"
        ], cwd=".", stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        
        print(f"🐕 Snoop Dog #{ai_id}: Started")
        return ai
        
    except Exception as e:
        print(f"❌ Snoop Dog #{ai_id}: Failed to start - {e}")
        return None

def connect_snoop_ais(count):
    """Start multiple SNOOP_DOG AIs"""
    ais = []
    
    print(f"🐕 Starting {count} Snoop Dogs...")
    
    for i in range(count):
        ai = start_ai(i + 1)
        if ai:
            ais.append(ai)
        else:
            print(f"❌ Failed to start Snoop Dog #{i + 1}")
        
        # Small delay between starts
        time.sleep(0.2)
    
    print(f"✅ Successfully started {len(ais)} Snoop Dogs")
    return ais

def test_snoop_connection():
    """Test basic connection to SNOOP_DOG server"""
    print("🔍 Testing connection to SNOOP_DOG server...")
    
    try:
        # Create socket connection
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect(("localhost", 4242))
        
        # Get welcome message
        welcome = s.recv(1024).decode()
        print(f"📨 Welcome message: {welcome.strip()}")
        
        # Test SNOOP_DOG team connection
        s.sendall(b"SNOOP_DOG\n")
        response = s.recv(1024).decode()
        print(f"📨 SNOOP_DOG response: {response.strip()}")
        
        # Should contain "49\n30 30" (49 remaining slots, 30x30 map)
        if "30 30" in response:
            print("✅ SNOOP_DOG server connection confirmed!")
        else:
            print("❌ Expected server response not found")
        
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
    print("=== Zappy Snoop Dog Stress Test ===")
    print("Server: 30x30 board, SNOOP_DOG team ONLY")
    print("Map: 30x30 world filled with Snoop Dogs")
    print("Team: SNOOP_DOG only (up to 50 players)")
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
    snoop_ais = []
    
    try:
        # Start server
        server = start_server()
        print("✅ Server started successfully")
        
        # Test connection to SNOOP_DOG server
        if not test_snoop_connection():
            print("❌ Connection test failed, stopping test")
            return
        
        # Start many SNOOP_DOG AIs (40 out of 50 possible)
        snoop_ais = connect_snoop_ais(40)
        
        total_started = len(snoop_ais)
        print(f"\n📊 Total Snoop Dogs started: {total_started}/40")
        
        if total_started == 40:
            print("🎉 SUCCESS: All 40 Snoop Dogs started!")
        else:
            print(f"⚠️  WARNING: Only {total_started}/40 Snoop Dogs started")
        
        print("\n🎮 Snoop Dog stress test is now running!")
        print("• Server: 30x30 world with SNOOP_DOG team only")
        print("• AIs: 40 Snoop Dogs roaming the world")
        print("• Visual: Green faces and Snoop Dog textures everywhere!")
        print("• Audio: Snoop Dog music should be playing")
        print("• Duration: Run as long as you want")
        print("\n💡 To watch the Snoop Dog party:")
        print("  • Open a new terminal")
        print("  • Run: ./zappy_gui -p 4242 -h localhost")
        print("  • Look for green Snoop Dog faces everywhere!")
        print("  • Listen for Snoop Dog music")
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
        
        # Stop all Snoop Dogs
        print("🐕 Stopping Snoop Dogs...")
        for ai in snoop_ais:
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