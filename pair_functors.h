#ifndef GAME_OF_LIFE_PAIR_FUNCTORS_H
#define GAME_OF_LIFE_PAIR_FUNCTORS_H

/* We need to define a custom hash and equal functors for pair type (for unordered_set and unordered_map).
 Defining a hash is not enough, since hash functions can have collisions.
 Side note: the overloaded '()' is templated, but that's okay, because C++ has type inference */

// Note to self: by moving to the new hash function, we've made map access faster by 80%-90%(!).
// *From testing of a lot of hashes, the following is good as well, albeit a bit slower than current one: (pair.x * 0x1f1f1f1f) ^ ~pair.y
class pair_hash{
public:
    std::size_t operator() (const sf::Vector2i &pair) const{
        std::hash<long long int> hash_obj;
        return hash_obj(pair.x * (INT_MAX + (long long int)1) + pair.y);
    }
};
class pair_equal{
public:
    bool operator() (const sf::Vector2i &pair1, const sf::Vector2i &pair2) const{
        return pair1.x == pair2.x && pair1.y == pair2.y;
    }
};

// We need to define a custom less functor for pair type (for set).
class pair_less{
public:
    // We define 'pair1' to be less than 'pair2' if the former is above the latter;
    // or if they're in the same line, and the former is to the left of the latter.
    bool operator() (const sf::Vector2i &pair1, const sf::Vector2i &pair2) const{
        if (pair1.y < pair2.y) return true;
        else if (pair1.y == pair2.y) return pair1.x < pair2.x;

        return false;
    }
};

#endif