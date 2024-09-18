"""
test.py - Test suite for the Mini-Redis server

This file contains various tests to ensure the correct functionality,
performance, and fault tolerance of the Mini-Redis server.
"""

import socket
import time
import tracemalloc
import random
import string
import pytest

PORT = 45234
BUFFER_SIZE = 1024

def setup_module(module):
    """Setup function to be run before all tests."""
    # Add any necessary setup code here, e.g., starting the server

def teardown_module(module):
    """Teardown function to be run after all tests."""
    # Add any necessary teardown code here, e.g., stopping the server

def send_command(command):
    """Helper function to send commands to the server and receive responses."""
    try:
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.connect(("127.0.0.1", PORT))
            s.sendall(command.encode())
            response = s.recv(BUFFER_SIZE).decode().strip()
        return response
    except socket.error as e:
        pytest.fail(f"Socket error occurred: {e}")

# Generate a random string for testing
def generate_random_string(length=10):
    """Generate a random string for testing purposes."""
    letters = string.ascii_letters
    return "".join(random.choice(letters) for i in range(length))

# Test basic CRUD operations
def test_basic_operations():
    """Test basic CRUD operations: SET, GET, and DEL."""
    # Test implementation remains the same

    # Test SET operation
    response = send_command('{"key": "key1", "operation": "SET", "value": "value1"}')
    assert response == "OK", f"SET command failed: {response}"

    # Test GET operation
    response = send_command('{"key": "key1", "operation": "GET"}')
    assert response == '"value1"', f"GET command failed: {response}"

    # Test DEL operation
    response = send_command('{"key": "key1", "operation": "DEL"}')
    assert response == "Deleted", f"DEL command failed: {response}"

    # Verify deletion
    response = send_command('{"key": "key1", "operation": "GET"}')
    assert response == "Not Found", f"GET command failed: {response}"

# Test AVL tree resizing and data integrity
def test_resize_avl_tree():
    """Test AVL tree resizing and data integrity with a large number of insertions."""
    # Test implementation remains the same

    # Insert 1000 key-value pairs
    for i in range(1000):
        key = f"key{i}"
        value = f"value{i}"
        response = send_command(
            f'{{"key": "{key}", "operation": "SET", "value": "{value}"}}'
        )
        assert response == "OK", f"SET command failed for {key}: {response}"

    # Verify all 1000 key-value pairs
    for i in range(1000):
        key = f"key{i}"
        expected_value = f"value{i}"
        response = send_command(f'{{"key": "{key}", "operation": "GET"}}')
        assert (
            response == f'"{expected_value}"'
        ), f"GET command failed for {key}: {response}"

# Test for potential memory leaks
def test_memory_leak():
    """Test for potential memory leaks during a large number of operations."""
    # Test implementation remains the same

    tracemalloc.start()

    # Perform a large number of operations
    for _ in range(1000):
        key = generate_random_string()
        value = generate_random_string()
        send_command(f'{{"key": "{key}", "operation": "SET", "value": "{value}"}}')

    current, peak = tracemalloc.get_traced_memory()
    tracemalloc.stop()

    print(f"Current memory usage: {current / 10**6}MB; Peak: {peak / 10**6}MB")
    assert current < 10 * 1024 * 1024, "Potential memory leak detected"

# Test server performance
def test_performance():
    """Test server performance by measuring time taken for multiple SET operations."""
    # Test implementation remains the same

    start_time = time.time()
    for i in range(10000):
        key = f"perfkey{i}"
        value = f"perfvalue{i}"
        send_command(f'{{"key": "{key}", "operation": "SET", "value": "{value}"}}')
    end_time = time.time()

    duration = end_time - start_time
    print(f"Time taken for 10000 SET operations: {duration:.2f} seconds")
    assert duration < 10, "Performance test failed: took too long"

# Test server fault tolerance
def test_fault_tolerance():
    """Test server fault tolerance against connection loss and partial data."""
    # Test implementation remains the same

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
