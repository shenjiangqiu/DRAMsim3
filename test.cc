
#include <memory_system.h>
#include <format>
int test_address_mapping();
int test2();
int test_read();
int main() {
    test_address_mapping();
    test_read();
    test2();
}

int test_read() {
    auto total_read = 0;
    auto total_write = 0;
    auto config = new dramsim3::Config("configs/HBM2_8Gb_x128.ini", ".");
    auto dram_sys = dramsim3::MemorySystem(
        config, ".", [&](int addr) { total_read++; },
        [&](int addr) { total_write++; });
    auto total_sent = 0;
    auto total_cycle = 0;
    auto channels = config->channels;
    auto ranks = config->ranks;
    auto bgs = config->bankgroups;
    auto banks = config->banks_per_group;

    while (total_write + total_read != 2000) {
        while (total_sent != 2000) {
            auto addr = total_sent * 64;
            if (!dram_sys.WillAcceptTransaction(addr, false)) {
                break;
            }
            auto good = dram_sys.AddTransaction(addr, false);
            if (good) {
                total_sent++;
            } else {
                break;
            }
        }
        dram_sys.ClockTick();
        total_cycle++;
    }
    std::cout << "Total read: " << total_read << std::endl;
    std::cout << "Total write: " << total_write << std::endl;
    std::cout << "Total sent: " << total_sent << std::endl;
    std::cout << "Total cycle: " << total_cycle << std::endl;
}

int test2() {
    auto total_read = 0;
    auto total_write = 0;
    auto config = new dramsim3::Config("configs/HBM2_8Gb_x128.ini", ".");
    auto dram_sys = dramsim3::MemorySystem(
        config, ".", [&](int addr) { total_read++; },
        [&](int addr) { total_write++; });
    auto total_sent = 0;
    auto total_cycle = 0;
    auto channels = config->channels;
    auto ranks = config->ranks;
    auto bgs = config->bankgroups;
    auto banks = config->banks_per_group;

    while (total_write + total_read != 2000) {
        while (total_sent != 2000) {
            auto i = total_sent;
            auto channel = i % channels;
            i = i / channels;
            auto rank = i % ranks;
            i = i / ranks;
            auto bg = i % bgs;
            i = i / bgs;
            auto bank = i % banks;
            i = i / banks;
            auto row = i;
            auto addr = config->GetHexAddress(
                dramsim3::Address(channel, rank, bg, bank, row, 0));
            if (!dram_sys.WillAcceptTransaction(addr, false)) {
                break;
            }
            auto good = dram_sys.AddTransaction(addr, false);
            if (good) {
                total_sent++;
            } else {
                break;
            }
        }
        dram_sys.ClockTick();
        total_cycle++;
    }
    std::cout << "Total read: " << total_read << std::endl;
    std::cout << "Total write: " << total_write << std::endl;
    std::cout << "Total sent: " << total_sent << std::endl;
    std::cout << "Total cycle: " << total_cycle << std::endl;
}

int test_address_mapping() {
    auto config = dramsim3::Config("configs/HBM2_8Gb_x128.ini", ".");
    for (int i = 0; i < 100; i++) {
        auto address = dramsim3::Address(0, 0, 0, 0, i, 0);
        auto hex_addr = config.GetHexAddress(address);
        auto mapped = config.AddressMapping(hex_addr);
        assert(mapped.channel == 0);
        assert(mapped.rank == 0);
        assert(mapped.bankgroup == 0);
        assert(mapped.bank == 0);
        assert(mapped.row == i);
        assert(mapped.column == 0);
    }
    return 0;
}