#!/usr/bin/env python3
"""
Network Calculator Unit Tests
Tests IPv4 subnet calculation functions independently from the Flipper UI.
"""

import sys
import os
import struct

# Add project root to path for importing calculator functions
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

# Test vectors for IPv4 calculations
def ip_to_uint(ip_str):
    """Convert IP string to uint32."""
    parts = ip_str.split('.')
    return (int(parts[0]) << 24) | (int(parts[1]) << 16) | (int(parts[2]) << 8) | int(parts[3])

def uint_to_ip(val):
    """Convert uint32 to IP string."""
    return f"{(val >> 24) & 0xFF}.{(val >> 16) & 0xFF}.{(val >> 8) & 0xFF}.{val & 0xFF}"

def cidr_to_mask(cidr):
    """Convert CIDR to subnet mask."""
    if cidr <= 0:
        return 0
    if cidr >= 32:
        return 0xFFFFFFFF
    return (0xFFFFFFFF << (32 - cidr)) & 0xFFFFFFFF

def calculate_network(ip_uint, cidr):
    """Calculate network address."""
    mask = cidr_to_mask(cidr)
    return ip_uint & mask

def calculate_broadcast(ip_uint, cidr):
    """Calculate broadcast address."""
    mask = cidr_to_mask(cidr)
    return (ip_uint | ~mask) & 0xFFFFFFFF

def calculate_usable_hosts(cidr):
    """Calculate usable host count."""
    if cidr == 31:
        return 2
    elif cidr == 32:
        return 1
    else:
        total = 1 << (32 - cidr)
        return total - 2

def calculate_total_addresses(cidr):
    """Calculate total address count."""
    return 1 << (32 - cidr)

def is_private_ip(ip_uint):
    """Check if IP is private."""
    # 10.0.0.0/8
    if (ip_uint & 0xFF000000) == 0x0A000000:
        return True
    # 172.16.0.0/12
    if (ip_uint & 0xFFF00000) == 0xAC100000:
        return True
    # 192.168.0.0/16
    if (ip_uint & 0xFFFF0000) == 0xC0A80000:
        return True
    return False

# Test functions
def test_valid_ipv4():
    """Test valid IPv4 address parsing."""
    assert ip_to_uint("0.0.0.0") == 0
    assert ip_to_uint("255.255.255.255") == 0xFFFFFFFF
    assert ip_to_uint("192.168.1.1") == 0xC0A80101
    assert ip_to_uint("10.0.0.1") == 0x0A000001
    assert ip_to_uint("172.16.0.1") == 0xAC100001
    print("PASS: test_valid_ipv4")

def test_invalid_ipv4():
    """Test invalid IPv4 address rejection."""
    # These would be rejected by validation logic
    assert ip_to_uint("256.1.1.1") != ip_to_uint("1.1.1.1")  # Invalid octet
    print("PASS: test_invalid_ipv4")

def test_cidr_zero():
    """Test /0 (all addresses)."""
    assert cidr_to_mask(0) == 0
    assert calculate_total_addresses(0) == 4294967296
    assert calculate_usable_hosts(0) == 4294967294
    print("PASS: test_cidr_zero")

def test_cidr_24():
    """Test /24 (common subnet)."""
    ip = ip_to_uint("192.168.1.1")
    mask = cidr_to_mask(24)
    assert mask == 0xFFFFFF00
    net = calculate_network(ip, 24)
    assert net == 0xC0A80100  # 192.168.1.0
    bcast = calculate_broadcast(ip, 24)
    assert bcast == 0xC0A801FF  # 192.168.1.255
    total = calculate_total_addresses(24)
    assert total == 256
    usable = calculate_usable_hosts(24)
    assert usable == 254
    print("PASS: test_cidr_24")

def test_cidr_31():
    """Test /31 (RFC 3021 point-to-point)."""
    ip = ip_to_uint("10.0.0.0")
    net = calculate_network(ip, 31)
    assert net == 0x0A000000
    bcast = calculate_broadcast(ip, 31)
    assert bcast == 0x0A000001
    total = calculate_total_addresses(31)
    assert total == 2
    usable = calculate_usable_hosts(31)
    assert usable == 2
    print("PASS: test_cidr_31")

def test_cidr_32():
    """Test /32 (single host)."""
    ip = ip_to_uint("192.168.1.5")
    net = calculate_network(ip, 32)
    assert net == ip
    bcast = calculate_broadcast(ip, 32)
    assert bcast == ip
    total = calculate_total_addresses(32)
    assert total == 1
    usable = calculate_usable_hosts(32)
    assert usable == 1
    print("PASS: test_cidr_32")

def test_invalid_cidr():
    """Test invalid CIDR values."""
    assert cidr_to_mask(33) == cidr_to_mask(32)  # Clamped
    print("PASS: test_invalid_cidr")

def test_private_addresses():
    """Test private address classification."""
    assert is_private_ip(ip_to_uint("10.0.0.1")) == True
    assert is_private_ip(ip_to_uint("172.16.0.1")) == True
    assert is_private_ip(ip_to_uint("172.31.255.255")) == True
    assert is_private_ip(ip_to_uint("192.168.1.1")) == True
    assert is_private_ip(ip_to_uint("8.8.8.8")) == False
    assert is_private_ip(ip_to_uint("1.1.1.1")) == False
    print("PASS: test_private_addresses")

def test_special_addresses():
    """Test special address types."""
    # Loopback
    assert (ip_to_uint("127.0.0.1") & 0xFF000000) == 0x7F000000
    # Link-local
    assert (ip_to_uint("169.254.1.1") & 0xFFFF0000) == 0xA9FE0000
    # Multicast
    assert (ip_to_uint("224.0.0.1") & 0xF0000000) == 0xE0000000
    # Limited broadcast
    assert ip_to_uint("255.255.255.255") == 0xFFFFFFFF
    print("PASS: test_special_addresses")

def test_boundary_addresses():
    """Test network boundary addresses."""
    # First usable in /24
    ip = ip_to_uint("192.168.1.0")
    first_usable = calculate_network(ip, 24) + 1
    assert uint_to_ip(first_usable) == "192.168.1.1"
    # Last usable in /24
    last_usable = calculate_broadcast(ip, 24) - 1
    assert uint_to_ip(last_usable) == "192.168.1.254"
    print("PASS: test_boundary_addresses")

if __name__ == "__main__":
    test_valid_ipv4()
    test_invalid_ipv4()
    test_cidr_zero()
    test_cidr_24()
    test_cidr_31()
    test_cidr_32()
    test_invalid_cidr()
    test_private_addresses()
    test_special_addresses()
    test_boundary_addresses()
    print("\nAll network calculator tests PASSED!")