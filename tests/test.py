import socket
import time
import tracemalloc
import random
import string
import pytest

PORT = 45234
BUFFER_SIZE = 1024


def send_command(command):
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect(("127.0.0.1", PORT))
        s.sendall(command.encode())
        response = s.recv(BUFFER_SIZE).decode().strip()
    return response


def generate_random_string(length=10):
    letters = string.ascii_letters
    return "".join(random.choice(letters) for i in range(length))


def test_basic_operations():
    # Set data
    response = send_command('{"key": "key1", "operation": "SET", "value": "value1"}')
    assert response == "OK", f"SET command failed: {response}"

    # Get data
    response = send_command('{"key": "key1", "operation": "GET"}')
    assert response == '"value1"', f"GET command failed: {response}"

    # Delete data
    response = send_command('{"key": "key1", "operation": "DEL"}')
    assert response == "Deleted", f"DEL command failed: {response}"

    # Get deleted data again
    response = send_command('{"key": "key1", "operation": "GET"}')
    assert response == "Not Found", f"GET command failed: {response}"


def test_resize_avl_tree():
    for i in range(1000):
        key = f"key{i}"
        value = f"value{i}"
        response = send_command(
            f'{{"key": "{key}", "operation": "SET", "value": "{value}"}}'
        )
        assert response == "OK", f"SET command failed for {key}: {response}"

    for i in range(1000):
        key = f"key{i}"
        expected_value = f"value{i}"
        response = send_command(f'{{"key": "{key}", "operation": "GET"}}')
        assert (
            response == f'"{expected_value}"'
        ), f"GET command failed for {key}: {response}"


def test_memory_leak():
    tracemalloc.start()

    for _ in range(1000):
        key = generate_random_string()
        value = generate_random_string()
        send_command(f'{{"key": "{key}", "operation": "SET", "value": "{value}"}}')

    current, peak = tracemalloc.get_traced_memory()
    tracemalloc.stop()

    print(f"Current memory usage: {current / 10**6}MB; Peak: {peak / 10**6}MB")
    assert current < 10 * 1024 * 1024, "Potential memory leak detected"


def test_performance():
    start_time = time.time()
    for i in range(10000):
        key = f"perfkey{i}"
        value = f"perfvalue{i}"
        send_command(f'{{"key": "{key}", "operation": "SET", "value": "{value}"}}')
    end_time = time.time()

    duration = end_time - start_time
    print(f"Time taken for 10000 SET operations: {duration:.2f} seconds")
    assert duration < 10, "Performance test failed: took too long"


# def test_connection_handling():
#     sockets = []
#     try:
#         for _ in range(100):
#             s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
#             s.connect(("127.0.0.1", PORT))
#             sockets.append(s)

#         assert len(sockets) == 100, "Failed to open 100 connections"
#     finally:
#         for s in sockets:
#             s.close()


def test_fault_tolerance():
    # Test connection loss
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect(("127.0.0.1", PORT))
    s.close()

    # Test sending partial data
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect(("127.0.0.1", PORT))
    s.send(b'{"key": "partial", "operation')
    s.close()

    # Ensure server is still responsive
    response = send_command('{"key": "test", "operation": "SET", "value": "test"}')
    assert response == "OK", "Server not responsive after fault tolerance test"


if __name__ == "__main__":
    pytest.main([__file__, "-v"])
