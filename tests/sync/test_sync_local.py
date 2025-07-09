import requests
import time

# simple sanity test assuming two servers running on localhost with different ports

def get_status(port):
    r = requests.get(f"http://127.0.0.1:{port}/status")
    return r.json()

def send_play(port, path, pos):
    requests.post(f"http://127.0.0.1:{port}/play", json={"path": path, "position": pos})

if __name__ == "__main__":
    status = get_status(56791)
    print("Server status", status)
    send_play(56792, status.get("path", ""), status.get("position", 0))
    time.sleep(1)
    print("Target status", get_status(56792))
