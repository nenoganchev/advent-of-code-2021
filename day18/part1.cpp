#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "string_utils.h"

using namespace std;

class snailfish_number {
public:
    snailfish_number() = delete;
    snailfish_number(const string &number_str);
    ~snailfish_number();
    snailfish_number(const snailfish_number &);
    snailfish_number(snailfish_number &&);
    snailfish_number &operator = (const snailfish_number &);
    snailfish_number &operator = (snailfish_number &&);

    snailfish_number &operator += (const snailfish_number &rhs);

    string to_string() const;
    uint64_t magnitude() const;

private:
    struct node {
        uint64_t value;
        node *p_parent;
        node *p_left;
        node *p_right;

        node() { this->p_parent = this->p_left = this->p_right = nullptr; }
    } *p_root_node;

    static node *parse_number(istringstream &is);
    static uint64_t calculate_magnitude(const node *p_root);
    static node *copy_tree(const node *p_root);
    static void deallocate_tree(node *p_root);
    static void attach_left(node *p_node, node *p_attached_subtree);
    static void attach_right(node *p_node, node *p_attached_subtree);
    static void print_number(const node *p_root, ostream &os);
    static void explode_pair(node *p_pair_node);
    static void split_number(node *p_number_node);
    static bool is_number_node(const node *p_node);
    static bool is_numbers_pair_node(const node *p_node);
    static node *find_too_nested_pair(node *p_root, int level);
    static node *find_too_big_number(node *p_root);
    static node *find_first_number_to_the_left(node *p_node);
    static node *find_first_number_to_the_right(node *p_node);
    static node *find_rightmost_number(node *p_node);
    static node *find_leftmost_number(node *p_node);
    static void replace_in_parent_and_deallocate(node *p_replaced_node, node *p_replacing_node);

    void reduce();
};

ostream &operator << (ostream &, const snailfish_number &);

vector<snailfish_number> parse_input(const string &filename);
bool is_odd(uint64_t number);


int main(int argc, char *argv[]) {
    if (argc < 2) {
        cout << "Usage: <exe> <filename>\n";
        return 1;
    }
    const string filename = argv[1];

    vector<snailfish_number> numbers = parse_input(filename);

    cout << "Parsed numbers:" << endl;
    for (const snailfish_number &number : numbers) {
        cout << "  " << number << endl;
    }
    cout << endl;

    cout << "Adding numbers:" << endl;
    snailfish_number sum = numbers[0];
    for (size_t i = 1; i < numbers.size(); ++i) {
        sum += numbers[i];
        cout << "  Partial sum: " << sum << endl;
    }
    cout << endl;

    cout << "Final sum: " << sum << " (magnitude: " << sum.magnitude() << ")" << endl;
}

vector<snailfish_number> parse_input(const string &filename) {
    ifstream input_file(filename);
    vector<snailfish_number> numbers;

    string line;
    while (getline(input_file, line)) {
        numbers.push_back(snailfish_number(line));
    }

    return numbers;
}

snailfish_number::snailfish_number(const string &number_str) {
    istringstream is(number_str);
    this->p_root_node = parse_number(is);

    if (is.peek() != istringstream::traits_type::eof()) throw runtime_error("Number string contains more data than expected");

    this->reduce();
}

snailfish_number::~snailfish_number() {
    deallocate_tree(this->p_root_node);
}

snailfish_number::snailfish_number(const snailfish_number &rhs) {
    this->p_root_node = copy_tree(rhs.p_root_node);
}

snailfish_number::snailfish_number(snailfish_number &&moved_from) {
    this->p_root_node = moved_from.p_root_node;
    moved_from.p_root_node = nullptr;
}

snailfish_number &snailfish_number::operator = (const snailfish_number &rhs) {
    if (this == &rhs) return *this;

    deallocate_tree(this->p_root_node);
    this->p_root_node = copy_tree(rhs.p_root_node);

    return *this;
}

snailfish_number &snailfish_number::operator = (snailfish_number &&rhs) {
    if (this == &rhs) return *this;

    deallocate_tree(this->p_root_node);
    this->p_root_node = rhs.p_root_node;
    rhs.p_root_node = nullptr;

    return *this;
}

snailfish_number &snailfish_number::operator += (const snailfish_number &rhs) {
    node *p_new_root = new node();

    attach_left(p_new_root, this->p_root_node);
    attach_right(p_new_root, copy_tree(rhs.p_root_node));
    this->p_root_node = p_new_root;

    //cout << "    After addition: " << *this << endl;
    this->reduce();

    return *this;
}

string snailfish_number::to_string() const {
    ostringstream os;
    print_number(this->p_root_node, os);
    return os.str();
}

uint64_t snailfish_number::magnitude() const {
    return calculate_magnitude(this->p_root_node);
}

snailfish_number::node *snailfish_number::parse_number(istringstream &is) {
    node *p_node = new node();

    if (is.peek() == '[') {
        is.get();
        attach_left(p_node, parse_number(is));
        is >> expect(",");
        attach_right(p_node, parse_number(is));
        is >> expect("]");
    }
    else {
        is >> p_node->value;
    }

    return p_node;
}

snailfish_number::node *snailfish_number::copy_tree(const node *p_node) {
    if (p_node == nullptr) return nullptr;

    node *p_copy = new node();
    p_copy->value = p_node->value;
    attach_left(p_copy, copy_tree(p_node->p_left));
    attach_right(p_copy, copy_tree(p_node->p_right));

    return p_copy;
}

void snailfish_number::deallocate_tree(node *p_node) {
    if (p_node == nullptr) return;

    deallocate_tree(p_node->p_left);
    deallocate_tree(p_node->p_right);
    delete p_node;
}

void snailfish_number::attach_left(node *p_node, node *p_attached_subtree) {
    p_node->p_left = p_attached_subtree;
    if (p_attached_subtree) p_attached_subtree->p_parent = p_node;
}

void snailfish_number::attach_right(node *p_node, node *p_attached_subtree) {
    p_node->p_right = p_attached_subtree;
    if (p_attached_subtree) p_attached_subtree->p_parent = p_node;
}

void snailfish_number::print_number(const node *p_node, ostream &os) {
    if (is_number_node(p_node)) {
        os << p_node->value;
    }
    else {
        os << "[";
        print_number(p_node->p_left, os);
        os << ",";
        print_number(p_node->p_right, os);
        os << "]";
    }
}

uint64_t snailfish_number::calculate_magnitude(const node *p_node) {
    if (is_number_node(p_node)) return p_node->value;

    return 3 * calculate_magnitude(p_node->p_left) + 2 * calculate_magnitude(p_node->p_right);
}

void snailfish_number::explode_pair(node *p_pair) {
    if (!is_numbers_pair_node(p_pair)) throw runtime_error("Exploding node is not a pair of numbers");
    if (p_pair->p_parent == nullptr) throw runtime_error("Exploding node is the root node");

    node *p_first_number_to_the_left = find_first_number_to_the_left(p_pair);
    if (p_first_number_to_the_left) {
        p_first_number_to_the_left->value += p_pair->p_left->value;
    }

    node *p_first_number_to_the_right = find_first_number_to_the_right(p_pair);
    if (p_first_number_to_the_right) {
        p_first_number_to_the_right->value += p_pair->p_right->value;
    }

    node *p_replacing_number = new node();
    p_replacing_number->value = 0;

    replace_in_parent_and_deallocate(p_pair, p_replacing_number);
}

void snailfish_number::split_number(node *p_number_node) {
    if (!is_number_node(p_number_node)) throw runtime_error("Split node is not a number node");

    node *p_replacing_pair = new node();
    attach_left(p_replacing_pair, new node());
    attach_right(p_replacing_pair, new node());

    p_replacing_pair->p_left->value = p_number_node->value / 2;
    p_replacing_pair->p_right->value = p_number_node->value / 2;
    if (is_odd(p_number_node->value)) {
        ++p_replacing_pair->p_right->value;
    }

    replace_in_parent_and_deallocate(p_number_node, p_replacing_pair);
}

bool snailfish_number::is_number_node(const node *p_node) {
    return p_node->p_left == nullptr && p_node->p_right == nullptr;
}

bool snailfish_number::is_numbers_pair_node(const node *p_node) {
    return !is_number_node(p_node) && is_number_node(p_node->p_left) && is_number_node(p_node->p_right);
}

snailfish_number::node *snailfish_number::find_too_nested_pair(node *p_node, int level) {
    if (is_number_node(p_node)) return nullptr;

    node *p_too_nested_pair = find_too_nested_pair(p_node->p_left, level + 1);
    if (p_too_nested_pair) return p_too_nested_pair;

    if (level > 4 && is_numbers_pair_node(p_node)) return p_node;

    return find_too_nested_pair(p_node->p_right, level + 1);
}

snailfish_number::node *snailfish_number::find_too_big_number(node *p_node) {
    if (is_number_node(p_node)) {
        return p_node->value >= 10 ? p_node : nullptr;
    }

    node *p_too_big_number = find_too_big_number(p_node->p_left);
    if (p_too_big_number) return p_too_big_number;
    return find_too_big_number(p_node->p_right);
}

snailfish_number::node *snailfish_number::find_first_number_to_the_left(node *p_node) {
    node *p_ancestor = p_node->p_parent;
    node *p_coming_from = p_node;
    while (p_ancestor && p_coming_from == p_ancestor->p_left) {
        p_coming_from = p_ancestor;
        p_ancestor = p_ancestor->p_parent;
    }
    if (p_ancestor == nullptr) return nullptr;

    return find_rightmost_number(p_ancestor->p_left);
}

snailfish_number::node *snailfish_number::find_first_number_to_the_right(node *p_node) {
    node *p_ancestor = p_node->p_parent;
    node *p_coming_from = p_node;
    while (p_ancestor && p_coming_from == p_ancestor->p_right) {
        p_coming_from = p_ancestor;
        p_ancestor = p_ancestor->p_parent;
    }
    if (p_ancestor == nullptr) return nullptr;

    return find_leftmost_number(p_ancestor->p_right);
}

snailfish_number::node *snailfish_number::find_rightmost_number(node *p_node) {
    if (is_number_node(p_node)) return p_node;

    return find_rightmost_number(p_node->p_right);
}

snailfish_number::node *snailfish_number::find_leftmost_number(node *p_node) {
    if (is_number_node(p_node)) return p_node;

    return find_leftmost_number(p_node->p_left);
}

void snailfish_number::replace_in_parent_and_deallocate(node *p_replaced_node, node *p_replacing_node) {
    if (p_replaced_node->p_parent->p_left == p_replaced_node) {
        attach_left(p_replaced_node->p_parent, p_replacing_node);
    }
    else if (p_replaced_node->p_parent->p_right == p_replaced_node) {
        attach_right(p_replaced_node->p_parent, p_replacing_node);
    }
    else {
        throw runtime_error("Unable to find replaced node among parent's children");
    }

    deallocate_tree(p_replaced_node);
}

void snailfish_number::reduce() {
    for (;;) {
        node *p_too_nested_pair = find_too_nested_pair(this->p_root_node, 1);
        if (p_too_nested_pair) {
            explode_pair(p_too_nested_pair);
            //cout << "    After explode:  " << *this << endl;
            continue;
        }

        node *p_too_big_number = find_too_big_number(this->p_root_node);
        if (p_too_big_number) {
            split_number(p_too_big_number);
            //cout << "    After split:    " << *this << endl;
            continue;
        }

        break;
    }
}

ostream &operator << (ostream &os, const snailfish_number &number) {
    os << number.to_string();
    return os;
}

bool is_odd(uint64_t number) {
    return (number & 1) != 0;
}
