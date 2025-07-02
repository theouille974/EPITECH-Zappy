import subprocess
import socket
import time

def start_server():
    server = subprocess.Popen(
        ["../../zappy_server", "-p", "4242", "-x", "10", "-y", "10", "-n", "team1", "team2", "-c", "3", "-f", "10"],
        cwd="src/server",
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE
    )
    time.sleep(1)
    return server

def stop_server(server):
    server.terminate()
    server.wait(timeout=10)

class ZappyClient:
    def __init__(self, host="localhost", port=4242):
        self.s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.s.connect((host, port))
        self.welcome = self.s.recv(1024).decode()

    def connect(self, team_name="team1"):
        response = self.send(team_name)
        response = response.strip() + "\n"
        try:
            response += self.s.recv(1024).decode()
        except socket.timeout:
            raise AssertionError("Timeout lors de la connexion (pas de réponse du serveur)")
        return response

    def send(self, msg, timeout=2):
        self.s.settimeout(timeout)
        self.s.sendall((msg + "\n").encode())
        try:
            response = self.s.recv(1024).decode()
        except socket.timeout:
            raise AssertionError(f"Timeout: aucune réponse du serveur pour la commande '{msg}'")
        return response
    
    def recive(self, size=1024):
        self.s.settimeout(2)
        try:
            response = self.s.recv(size).decode()
        except socket.timeout:
            raise AssertionError("Timeout: aucune réponse du serveur")
        return response

    def close(self):
        self.s.close()

def test_server_accepts_connection():
    server = start_server()
    try:
        client = ZappyClient()
        assert "WELCOME" in client.welcome
        client.close()
    finally:
        stop_server(server)

def test_server_accepts_multiple_connections():
    server = start_server()
    try:
        client1 = ZappyClient()
        client2 = ZappyClient()
        assert "WELCOME" in client1.welcome
        assert "WELCOME" in client2.welcome
        client1.close()
        client2.close()
    finally:
        stop_server(server)

def test_server_join_command():
    server = start_server()
    try:
        client = ZappyClient()
        response = client.connect("team1")
        assert "2\n10 10" in response.lower()
        client.close()
    finally:
        stop_server(server)

def test_server_join_command_invalid_team():
    server = start_server()
    try:
        client = ZappyClient()
        response = client.send("invalid_team")
        assert "ko" in response.lower()
        client.close()
    finally:
        stop_server(server)

def test_server_join_command_multiple_teams():
    server = start_server()
    try:
        client1 = ZappyClient()
        response1 = client1.connect("team1")
        assert "2\n10 10" in response1.lower()
        
        client2 = ZappyClient()
        response2 = client2.connect("team2")
        assert "2\n10 10" in response2.lower()
        
        client1.close()
        client2.close()
    finally:
        stop_server(server)

def test_server_join_command_full_team():
    server = start_server()
    try:
        client1 = ZappyClient()
        response1 = client1.connect("team1")
        assert "2\n10 10" in response1.lower()
        
        client2 = ZappyClient()
        response2 = client2.connect("team1")
        assert "1\n10 10" in response2.lower()

        client3 = ZappyClient()
        response3 = client3.connect("team1")
        assert "0\n10 10" in response3.lower()  

        client4 = ZappyClient()
        response4 = client4.connect("team1")
        assert "ko" in response4.lower()  # Team is full

        client1.close()
        client2.close()
        client3.close()
        client4.close()
    finally:
        stop_server(server)

def test_forward_command():
    server = start_server()
    try:
        client = ZappyClient()
        client.connect("team1")
        response = client.send("Forward")
        assert "ok" in response.lower()
        client.close()
    finally:
        stop_server(server)

def test_right_command():
    server = start_server()
    try:
        client = ZappyClient()
        client.connect("team1")
        response = client.send("Right")
        assert "ok" in response.lower()
        client.close()
    finally:
        stop_server(server)

def test_left_command():
    server = start_server()
    try:
        client = ZappyClient()
        client.connect("team1")
        response = client.send("Left")
        assert "ok" in response.lower()
        client.close()
    finally:
        stop_server(server)

def test_broadcast_command():
    server = start_server()
    try:
        client = ZappyClient()
        client.connect("team1")
        response = client.send("Broadcast Hello")
        assert "message 0, Hello" in response
        client.close()
    finally:
        stop_server(server)

def test_broadcast_from_other_team():
    server = start_server()
    try:
        client = ZappyClient()
        client.connect("team2")
        client2 = ZappyClient()
        client2.connect("team1")
        client.send("Broadcast Hello from team2")
        response = client2.recive()
        assert ", Hello from team2" in response
        client2.close()
        client.close()
    finally:
        stop_server(server)

def test_Connect_nbr():
    server = start_server()
    try:
        client = ZappyClient()
        client.connect("team1")
        response = client.send("Connect_nbr")
        print(response)
        assert "2" in response.strip()
        client.close()
    finally:
        stop_server(server)

def test_Incantation():
    server = start_server()
    try:
        client = ZappyClient()
        client.connect("team1")
        response = client.send("Incantation")
        assert "ko" in response.lower() or "Elevation underway" in response
        if "Elevation underway" in response:
            time.sleep(2)  # Wait for the incantation to complete
            response = client.send("Incantation")
            assert "ko" in response.lower() or "Elevation underway" in response
        client.close()
    finally:
        stop_server(server)

def test_take_object():
    server = start_server()
    try:
        client = ZappyClient()
        client.connect("team1")
        response = "ko"
        max = 10
        while "ko" in response.lower() and max > 0:
            client.send("Forward")
            response = client.send("Take food")
            assert "ok" in response.lower() or "ko" in response.lower()
            max -= 1
        client.close()
    finally:
        stop_server(server)

def test_invalid_team_name():
    server = start_server()
    try:
        client = ZappyClient()
        response = client.send("invalid_team")
        assert "ko" in response.lower()
        client.close()
    finally:
        stop_server(server)

def test_set_down_item():
    server = start_server()
    try:
        client = ZappyClient()
        client.connect("team1")
        response = client.send("Set food")
        assert "ok" in response.lower()
        client.close()
    finally:
        stop_server(server)

def test_invalid_set_down_item():
    server = start_server()
    try:
        client = ZappyClient()
        client.connect("team1")
        response = client.send("Set linemate")
        assert "ko" in response.lower()
        client.close()
    finally:
        stop_server(server)

def test_invalid_command():
    server = start_server()
    try:
        client = ZappyClient()
        client.connect("team1")
        response = client.send("foobar")
        assert "ko" in response.lower()
        client.close()
    finally:
        stop_server(server)

def test_inventory():
    server = start_server()
    try:
        client = ZappyClient()
        client.connect("team1")
        response = client.send("Inventory")
        print(response)
        assert "[ food 10, linemate 0, deraumere 0, sibur 0, mendiane 0, phiras 0, thystame 0, life 138 ]" in response
        client.close()
    finally:
        stop_server(server)

def test_pickup_inventory():
    server = start_server()
    try:
        client = ZappyClient()
        client.connect("team1")
        max = 10
        client.send("Forward")
        response = client.send("Take linemate")
        while "ko" in response.lower() and max > 0:
            client.send("Forward")
            response = client.send("Take linemate")
            assert "ok" in response.lower() or "ko" in response.lower()
            max -= 1
        response = client.send("Inventory")
        assert "linemate 1" in response
        client.close()
    finally:
        stop_server(server)

def test_look():
    server = start_server()
    try:
        client = ZappyClient()
        client.connect("team1")
        response = client.send("Look")
        print(response)
        assert "[player" and "food" and "]" in response and response.count(",") == 3
        client.close()
    finally:
        stop_server(server)

def test_complete_incantation():
    server = start_server()
    try:
        client = ZappyClient()
        client.connect("team1")
        response = client.send("Incantation")
        while "ko" in response.lower():
            client.send("Forward")
            response = client.send("Incantation")
            assert "elevation underway" in response.lower() or "ko" in response.lower()
        client.close()
    finally:
        stop_server(server)

def test_look_after_incantation():
    server = start_server()
    try:
        client = ZappyClient()
        client.connect("team1")
        response = client.send("Look")
        assert "[player" and "food" and "]" in response and response.count(",") == 3
        client.send("Incantation")
        while "ko" in response.lower():
            client.send("Forward")
            response = client.send("Incantation")
            assert "elevation underway" in response.lower() or "ko" in response.lower()
        response = client.send("Look")
        assert "[player" and "food" and "]" in response and response.count(",") == 3
        client.close()
    finally:
        stop_server(server)

if __name__ == "__main__":
    # test_server_accepts_connection()
    # test_server_join_command()
    # test_forward_command()
    # test_right_command()
    # test_left_command()
    # test_broadcast_command()
    # test_Connect_nbr()
    # test_Incantation()
    # test_take_object()
    # test_invalid_team_name()
    # test_set_down_item()
    # test_invalid_set_down_item()
    # test_invalid_command()
    test_inventory()
    # test_pickup_inventory()
    # test_look()
    print("All tests passed.")
