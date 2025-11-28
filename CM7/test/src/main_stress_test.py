# ============================
#   MicroPython Stress Test
# ============================

import time

# ---------- Workload Functions ----------

def stress_arithmetic(iterations):
    x = 0
    for i in range(iterations):
        x += (i * 3) // 2
        x ^= (i << 1)
        x -= i % 7
        if x < 0:
            x = -x
    return x

def stress_lists(size, passes):
    arr = list(range(size))
    checksum = 0
    for _ in range(passes):
        arr.reverse()

        first = arr[0]
        for i in range(size - 1):
            arr[i] = arr[i + 1]
        arr[-1] = first

        for v in arr:
            if v & 1:
                checksum += v
            else:
                checksum ^= v
    return checksum

def is_prime(n):
    if n < 2:
        return False
    for i in range(2, int(n ** 0.5) + 1):
        if n % i == 0:
            return False
    return True

def stress_primes(limit):
    count = 0
    for n in range(limit):
        if is_prime(n):
            count += 1
    return count

def fib(n):
    a, b = 0, 1
    for _ in range(n):
        a, b = b, a + b
    return a

def stress_fib(iterations):
    s = 0
    for i in range(iterations):
        s ^= fib(i % 20)
    return s


# ---------- Known-Good Validation ----------
EXPECTED_ARITH = stress_arithmetic(2000)
EXPECTED_LISTS = stress_lists(200, 10)
EXPECTED_PRIME = stress_primes(500)
EXPECTED_FIB   = stress_fib(200)


# ============================
#   Main Stress Loop
# ============================

def run_stress_test(duration_sec=10):
    print("Starting stress test for", duration_sec, "seconds")
    start = time.ticks_ms()

    ops = 0
    minutes_checked = 0   # How many minute boundaries passed

    try:
        while time.ticks_diff(time.ticks_ms(), start) < duration_sec * 1000:

            now = time.ticks_diff(time.ticks_ms(), start)

            # ---- Detect minute boundary crossing ----
            if now // 60000 > minutes_checked:
                minutes_checked = now // 60000
                print("Minute", minutes_checked, "passed. ops =", ops)

            # ---- Stress workloads ----
            a = stress_arithmetic(2000)
            b = stress_lists(200, 10)
            c = stress_primes(500)
            d = stress_fib(200)

            # ---- Validation Checks ----
            if a != EXPECTED_ARITH:
                raise ValueError("Arithmetic mismatch: got %d expected %d" % (a, EXPECTED_ARITH))

            if b != EXPECTED_LISTS:
                raise ValueError("List checksum mismatch: got %d expected %d" % (b, EXPECTED_LISTS))

            if c != EXPECTED_PRIME:
                raise ValueError("Prime counter mismatch: got %d expected %d" % (c, EXPECTED_PRIME))

            if d != EXPECTED_FIB:
                raise ValueError("Fibonacci mismatch: got %d expected %d" % (d, EXPECTED_FIB))

            ops += 1

    except Exception as e:
        print("ERROR DETECTED! Stress test aborted.")
        print("Reason:", e)
        print("Completed operations:", ops)
        return

    print("Stress test completed successfully.")
    print("Total operations:", ops)


# ============================
#   Execute
# ============================

run_stress_test(21600)   # run the test
