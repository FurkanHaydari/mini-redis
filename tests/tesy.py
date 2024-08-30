import socket
import time
import tracemalloc
import random
import string

PORT = 45234
BUFFER_SIZE = 1024

def send_command(command):
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect(('127.0.0.1', PORT))
        s.sendall(command.encode())
        response = s.recv(BUFFER_SIZE).decode().strip()  # Get response and strip whitespace
    return response

def generate_random_string(length=10):
    """Generates a random string."""
    letters = string.ascii_letters
    return ''.join(random.choice(letters) for i in range(length))

def test_basic_operations():
    # Set data
    response = send_command("SET key1 value1")
    assert response == "OK", f"SET command failed: {response}"

    # Get data
    response = send_command("GET key1")
    assert response == "value1", f"GET command failed: {response}"

    # Delete data
    response = send_command("DEL key1")
    assert response == "Deleted", f"DEL command failed: {response}"

    # Get deleted data again
    response = send_command("GET key1")
    assert response == "Not Found", f"GET command failed: {response}"

    print("Basic tests passed!")

def test_resize_avl_tree():
    # Start with a small database size
    response = send_command("SET key1 value1")
    assert response == "OK", f"SET command failed: {response}"

    # Add more data and ensure the AVL tree is balanced
    for i in range(2, 1001):  # Example of adding 1000 data entries
        key = f"key{i}"
        value = f"value{i}"
        response = send_command(f"SET {key} {value}")
        assert response == "OK", f"SET command failed: {response}"

    # Check data in the database
    for i in range(1, 1001):
        key = f"key{i}"
        expected_value = f"value{i}"
        response = send_command(f"GET {key}")
        assert response == expected_value, f"GET command failed: {response}"

    print("AVL tree expansion tests passed!")

def test_memory_leak():
    tracemalloc.start()
    
    # Add data for an extended period
    for i in range(100000):
        send_command(f"SET key{i} value{i}")

    # Check for memory leaks
    snapshot = tracemalloc.take_snapshot()
    top_stats = snapshot.statistics('lineno')

    print("Memory usage statistics:")
    for stat in top_stats[:10]:
        print(stat)

def test_performance():
    start_time = time.time()

    # Add large amounts of data
    for i in range(10000):
        send_command(f"SET key{i} value{i}")

    end_time = time.time()
    print(f"Performance test duration: {end_time - start_time} seconds")

def test_connection_handling():
    import threading
    
    def client_thread():
        response = send_command("SET key_concurrent value_concurrent")
        assert response == "OK", f"SET command failed: {response}"

        response = send_command("GET key_concurrent")
        assert response == "value_concurrent", f"GET command failed: {response}"

    threads = []
    for _ in range(10):  # 10 parallel clients
        t = threading.Thread(target=client_thread)
        threads.append(t)
        t.start()
    
    for t in threads:
        t.join()

    print("Connection handling tests passed!")

def test_fault_tolerance():
    # Test connection loss scenario
    try:
        response = send_command("SET key_fault_tolerance value")
    except Exception as e:
        print(f"Connection error: {e}")

    # Simulate network delay
    import time
    time.sleep(5)  # Wait for 5 seconds

    # Check the return value
    response = send_command("GET key_fault_tolerance")
    
    # If the expected "Not Found" response is not returned, "value" is returned, the server might still be holding the data
    # It is important to check if there are any remnants in the database
    if response == "value":
        print("Data is still present.")
    else:
        assert response == "Not Found", f"GET command failed: {response}"

    print("Fault tolerance tests passed!")

def test_randomized_operations():
    keys_values = {}
    
    # Perform SET and GET operations with random key-value pairs
    for _ in range(1000):
        key = generate_random_string()
        value = generate_random_string()
        keys_values[key] = value
        
        # Perform SET operation
        response = send_command(f"SET {key} {value}")
        assert response == "OK", f"SET command failed: {response}"
    
    for key, expected_value in keys_values.items():
        # Perform GET operation
        response = send_command(f"GET {key}")
        assert response == expected_value, f"GET command failed: {response}"
    
    print("Randomized operations test passed!")

if __name__ == "__main__":
    test_basic_operations()
    time.sleep(3)
    test_resize_avl_tree()
    time.sleep(3)
    test_memory_leak()
    time.sleep(3)
    test_performance()
    time.sleep(3)
    test_connection_handling()
    time.sleep(3)
    test_fault_tolerance()
    time.sleep(3)
    test_randomized_operations()
