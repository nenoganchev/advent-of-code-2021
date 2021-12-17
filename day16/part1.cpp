#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

struct packet_t {
    uint8_t version;
    enum {value_type, operator_type} type;
    uint64_t value;
    vector<packet_t> subpackets;
};

vector<bool> parse_input(const string &filename);
packet_t decode_outermost_packet(const vector<bool> &message);
packet_t decode_packet(const vector<bool> &message, size_t &pos);
int add_version_numbers(const packet_t &packet);

// Notes:
// 1. Single packet at outermost layer
// 2. Message may contain trailing zeroes that should be ignored. Not all trailing zeroes are discardable, though.
//
// Packet structure
// Header
// [3] Version
// [3] Type ID
// Literal value (type ID == 4)
//   [n * 5] Single binary number encoded as groups of 4 bits prefixed by 1 or 0 (for the last group): (1xxxx)*0xxxx
// Operator (type ID != 4)
//   [1] Length type ID
//   Bits length (length type ID == 0)
//     [15] Length of all sub-packets in bits
//   Sub-packets length (length type ID == 1)
//     [11] Number of all sub-packets
//   [Sub-packets]

int main(int argc, char *argv[]) {
    if (argc < 2) {
        cout << "Usage: <exe> <filename>\n";
        return 1;
    }
    const string filename = argv[1];

    vector<bool> message = parse_input(filename);
    packet_t outermost_packet = decode_outermost_packet(message);

    cout << "Version numbers sum: " << add_version_numbers(outermost_packet) << endl;
}

vector<bool> parse_input(const string &filename) {
    ifstream input_file(filename);
    vector<bool> bits;

    char hex_digit;
    while (input_file >> hex_digit) {
        switch (hex_digit) {
        case '0':
            bits.insert(bits.end(), {0, 0, 0, 0});
            break;
        case '1':
            bits.insert(bits.end(), {0, 0, 0, 1});
            break;
        case '2':
            bits.insert(bits.end(), {0, 0, 1, 0});
            break;
        case '3':
            bits.insert(bits.end(), {0, 0, 1, 1});
            break;
        case '4':
            bits.insert(bits.end(), {0, 1, 0, 0});
            break;
        case '5':
            bits.insert(bits.end(), {0, 1, 0, 1});
            break;
        case '6':
            bits.insert(bits.end(), {0, 1, 1, 0});
            break;
        case '7':
            bits.insert(bits.end(), {0, 1, 1, 1});
            break;
        case '8':
            bits.insert(bits.end(), {1, 0, 0, 0});
            break;
        case '9':
            bits.insert(bits.end(), {1, 0, 0, 1});
            break;
        case 'A':
            bits.insert(bits.end(), {1, 0, 1, 0});
            break;
        case 'B':
            bits.insert(bits.end(), {1, 0, 1, 1});
            break;
        case 'C':
            bits.insert(bits.end(), {1, 1, 0, 0});
            break;
        case 'D':
            bits.insert(bits.end(), {1, 1, 0, 1});
            break;
        case 'E':
            bits.insert(bits.end(), {1, 1, 1, 0});
            break;
        case 'F':
            bits.insert(bits.end(), {1, 1, 1, 1});
            break;
        default:
            throw runtime_error("Invalid input character");
        }
    }

    return bits;
}

packet_t decode_outermost_packet(const vector<bool> &message) {
    size_t pos = 0;
    packet_t outermost_packet = decode_packet(message, pos);

    for (; pos < message.size(); ++pos) {
        if (message[pos]) throw runtime_error("Found 1's after outermost layer");
    }

    return outermost_packet;
}

packet_t decode_packet(const vector<bool> &message, size_t &pos) {
    uint8_t version = 0;
    for (int i = 2; i >= 0; --i) {
        if (message.at(pos++)) version |= (1 << i);
    }
    uint8_t type = 0;
    for (int i = 2; i >= 0; --i) {
        if (message.at(pos++)) type |= (1 << i);
    }

    packet_t decoded_packet = {.version = version, .type = (type == 4 ? packet_t::value_type : packet_t::operator_type), .value = 0};

    if (decoded_packet.type == packet_t::value_type) {
        int bit_groups = 1;
        for (size_t i = 0; message.at(pos + i); i += 5) ++bit_groups;
        if (bit_groups > 16) throw runtime_error("Value doesn't fit into 64-bit integer");

        int bit_pos = (bit_groups * 4) - 1;
        for (int i = 0; i < bit_groups; ++i) {
            ++pos;  // skip group prefix bit
            for (int j = 0; j < 4; ++j, --bit_pos) {
                if (message.at(pos++)) decoded_packet.value |= (1 << bit_pos);
            }
        }
    }
    else {
        bool length_type_id = message.at(pos++);
        if (length_type_id == 0) {
            size_t subpackets_length = 0;
            for (int i = 14; i >= 0; --i) {
                if (message.at(pos++)) subpackets_length |= (1 << i);
            }

            size_t subpackets_start_pos = pos;
            while (pos - subpackets_start_pos < subpackets_length) {
                decoded_packet.subpackets.push_back(decode_packet(message, pos));
            }
            if (subpackets_start_pos + subpackets_length != pos) throw runtime_error("Decoded subpackets size doesn't match");
        }
        else {
            int subpackets_count = 0;
            for (int i = 10; i >= 0; --i) {
                if (message.at(pos++)) subpackets_count |= (1 << i);
            }

            for (int i = 0; i < subpackets_count; ++i) {
                decoded_packet.subpackets.push_back(decode_packet(message, pos));
            }
        }
    }

    return decoded_packet;
}

int add_version_numbers(const packet_t &packet) {
    int version_numbers_sum = packet.version;

    if (packet.type == packet_t::operator_type) {
        for (const packet_t &subpacket : packet.subpackets) {
            version_numbers_sum += add_version_numbers(subpacket);
        }
    }

    return version_numbers_sum;
}
