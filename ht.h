#ifndef HT_H
#define HT_H
#include <vector>
#include <iostream>
#include <cmath>

typedef size_t HASH_INDEX_T;


// Complete - Base Prober class
template <typename KeyType>
struct Prober {
    // Data members
    HASH_INDEX_T start_;    // initial hash location, h(k)
    HASH_INDEX_T m_;        // table size
    size_t numProbes_;      // probe attempts for statistic tracking
    static const HASH_INDEX_T npos = (HASH_INDEX_T)-1; // used to indicate probing failed
    void init(HASH_INDEX_T start, HASH_INDEX_T m, const KeyType& key) 
    {
        (void) key;  // avoid unused argument warnings since base class doesn't use key
        start_ = start;
        m_ = m;
        numProbes_ = 0;
    }
    HASH_INDEX_T next() {
        throw std::logic_error("Not implemented...should use derived class");
    }
};

// Almost Complete - Fill in the if statement below.
template <typename KeyType>
struct LinearProber : public Prober<KeyType> {

    HASH_INDEX_T next() 
    {
        // Complete the condition below that indicates failure
        // to find the key or an empty slot
        /* Fill me in */ 
        // ?*
        // If we are back at our original location
        if (this->numProbes_ == this->m_) {
            return this->npos; 
        }
        HASH_INDEX_T loc = (this->start_ + this->numProbes_) % this->m_;
        this->numProbes_++;
        return loc;
    }
};

// To be completed
template <typename KeyType, typename Hash2>
struct DoubleHashProber : public Prober<KeyType> 
{
    Hash2 h2_;              /// h2(k)
    HASH_INDEX_T dhstep_;   /// Stepsize to use for double hash probing

    /// Moduli to use for double hashing as table increases (resizes)
    static const HASH_INDEX_T DOUBLE_HASH_MOD_VALUES[]; 
    /// The number of elements in the array above
    static const int DOUBLE_HASH_MOD_SIZE;

    //==================================
    // Add data members, as desired
    //==================================

private:
    // Complete
    HASH_INDEX_T findModulusToUseFromTableSize(HASH_INDEX_T currTableSize)
    {
        HASH_INDEX_T modulus = DOUBLE_HASH_MOD_VALUES[0];
        // find the modulus that is just smaller than the table size
        for(int i=0; i < DOUBLE_HASH_MOD_SIZE && DOUBLE_HASH_MOD_VALUES[i] < currTableSize; i++)
        {
            modulus = DOUBLE_HASH_MOD_VALUES[i];
        }
        return modulus;
    }
public:
    /**
     * @brief Construct a new Double Hash Prober
     *     Accepts information that must be provided outside the hash table 
     *      (i.e. apart from internal hash table info/parameters)
     *        
     * 
     * @param h2  h2(k) - Object with an operator()(const KeyType&) defined for it
     */
    DoubleHashProber(const Hash2& h2 = Hash2()) : h2_(h2) {}
    
    /**
     * @brief Supplies info the hash table must provide
     * 
     * @param start Starting location for probing (i.e. h1(k))
     * @param m     Table size
     * @param key   Key (in case further hashing is necessary)
     */
    // Complete
    void init(HASH_INDEX_T start, HASH_INDEX_T m, const KeyType& key) 
    {
        Prober<KeyType>::init(start, m, key);
        HASH_INDEX_T modulus = findModulusToUseFromTableSize(m);
        // Compute probe stepsize given modulus and h2(k) 
        dhstep_ = modulus - h2_(key) % modulus;
    }

    // To be completed
    HASH_INDEX_T next() 
    {
        if (this->numProbes_ == this->m_){
            return this->npos; 
        }

        HASH_INDEX_T loc = (this->start_ + (this->numProbes_ * dhstep_) ) % this->m_;
        // std::cout << this->m_ << " " << loc << std::endl;

        this->numProbes_++; 
        return loc; 

    }
}; 

// Initialization of static array (do not alter)
template <typename KeyType, typename Hash2>
const HASH_INDEX_T DoubleHashProber<KeyType, Hash2>::DOUBLE_HASH_MOD_VALUES[] =
{
    7, 19, 43, 89, 193, 389, 787, 1583, 3191, 6397, 12841, 25703, 51431, 102871,
    205721, 411503, 823051, 1646221, 3292463, 6584957, 13169963, 26339921, 52679927,
    105359939, 210719881, 421439749, 842879563, 1685759113
};

// Initialization of static array size (do not alter)
template <typename KeyType, typename Hash2>
const int DoubleHashProber<KeyType, Hash2>::DOUBLE_HASH_MOD_SIZE = 
    sizeof(DoubleHashProber<KeyType, Hash2>::DOUBLE_HASH_MOD_VALUES)/sizeof(HASH_INDEX_T);

// Hash Table Interface
template<
    typename K, 
    typename V, 
    typename Prober = LinearProber<K>,
    typename Hash = std::hash<K>, 
    typename KEqual = std::equal_to<K> >
class HashTable
{
public:
    typedef K KeyType;
    typedef V ValueType;
    typedef std::pair<KeyType, ValueType> ItemType;
    typedef Hash Hasher;
    struct HashItem {
        ItemType item;
        bool deleted;
        HashItem(const ItemType& newItem){
            item = newItem;
            deleted = false;
        }
    };

    /**
     * @brief Construct a new Hash Table object
     * 
     * @param resizeAlpha Loading factor threshold at which the table should resize
     * @param prober Probing object of type Prober
     * @param hash Hash functor that supports hash(key) and returns a HASH_INDEX_T
     * @param kequal Functor that checks equality of two KeyType objects
     */
    HashTable(
        double resizeAlpha = 0.4, 
        const Prober& prober = Prober(),
        const Hasher& hash = Hasher(), 
        const KEqual& kequal = KEqual());

    /**
     * @brief Destroy the Hash Table object and delete all remaining
     *         key,value pairs
     * 
     */
    ~HashTable();

    /**
     * @brief Returns true if the table has no non-deleted key,value pairs,
     *        and false otherwise
     * 
     */
    bool empty() const;

    /**
     * @brief Returns number of (non-deleted) key,value pairs in the table
     * 
     * @return size_t 
     */
    size_t size() const;

    /**
     * @brief Inserts a new item into the map, or, if an item with the
     *        given key already exists, it updates the Value of that item
     *        with the second value of the pair, p
     * 
     * @param p Pair to insert  
     * @throw std::logic_error If no free location can be found
     */
    void insert(const ItemType& p);

    /**
     * @brief Removes (marks as deleted) the item with the given key.  
     * Does nothing if an item with the given key does not exist.
     * 
     * @param key 
     */
    void remove(const KeyType& key);

    /**
     * @brief Finds an item with the given key and returns a pointer 
     * to the key,value pair
     * 
     * @param key 
     * @return ItemType const* nullptr is returned if the key does not exist
     */
    ItemType const * find(const KeyType& key) const;
    ItemType * find(const KeyType& key);

    /**
     * @brief Returns the value corresponding to the given key
     * 
     * @param key 
     * throw std::out_of_range if the key does not exist
     * @return ValueType Value associated with key
     */
    const ValueType& at(const KeyType& key) const;
    ValueType& at(const KeyType& key);
    const ValueType& operator[](const KeyType& key) const;
    ValueType& operator[](const KeyType& key);

    // Debug / Performance functions
    void reportAll(std::ostream& out) const;
    void clearTotalProbes() { totalProbes_ = 0; }
    size_t totalProbes() const { return totalProbes_; }
private:
    /**
     * @brief Helper routine to find a given key
     * 
     * @param key 
     * @return HashItem* returns nullptr if key does not exist
     */
    HashItem * internalFind(const KeyType& key) const;
    /**
     * @brief Performs the probing sequence and returns the index
     * of the table location with the given key or the location where
     * key can be inserted (i.e. the index now contains nullptr) but is
     * available.
     * 
     * @param key 
     * @return returns npos is the key does not exist and
     * no free location is available
     */
    HASH_INDEX_T probe(const KeyType& key) const;

    // Constant to signify an invalid hash location is being returned
    static const HASH_INDEX_T npos = Prober::npos;

    /**
     * @brief Resizes the hash table replacing the old with a new
     * table of the next prime size given in CAPACITIES.  Must rehash
     * all non-deleted items while freeing all deleted items.
     * 
     * Must run in O(m) where m is the new table size
     * 
     * @throws std::logic_error if no more CAPACITIES exist
     */
    void resize();

    // Data members
    std::vector<HashItem*> table_; // actual hash table
    Hasher hash_;   
    KEqual kequal_;
    mutable Prober prober_;  // mutable allows const member functions to modify this member
    // debug/performance counters
    mutable size_t totalProbes_; // mutable allows const member functions to modify this member
    // prime capacities to be used when resizing/rehashing is needed
    static const HASH_INDEX_T CAPACITIES[];
    HASH_INDEX_T mIndex_;  // index to CAPACITIES

    // ADD MORE DATA MEMBERS HERE, AS NECESSARY
    HASH_INDEX_T capacitiesSize; // Size of capacities array
    double maxLoadingFactor; // resizeAlpha
    double currLoadingFactor; 

    HASH_INDEX_T currItems; 
    HASH_INDEX_T totalItems; 
    int strike; 

};

// ----------------------------------------------------------------------------
//                           Hash Table Implementation
// ----------------------------------------------------------------------------

// Static array of prime table sizes
template<typename K, typename V, typename Prober, typename Hash, typename KEqual>
const HASH_INDEX_T HashTable<K,V,Prober,Hash,KEqual>::CAPACITIES[] =
    {
        11, 23, 47, 97, 197, 397, 797, 1597, 3203, 6421, 12853, 25717, 51437, 102877,
        205759, 411527, 823117, 1646237, 3292489, 6584983, 13169977, 26339969, 52679969,
        105359969, 210719881, 421439783, 842879579, 1685759167
    };

// To be completed
template<typename K, typename V, typename Prober, typename Hash, typename KEqual>
HashTable<K,V,Prober,Hash,KEqual>::HashTable(
    double resizeAlpha, const Prober& prober, const Hasher& hash, const KEqual& kequal)
       :  hash_(hash), kequal_(kequal), prober_(prober)
{
    // Initialize any other data members as necessary
    
    
    this->mIndex_ = 0; 
    this->table_.resize(CAPACITIES[mIndex_]); 

    maxLoadingFactor = resizeAlpha; 
    
    currLoadingFactor = 0; 
    currItems = 0; 
    totalItems = 0; 

    strike = 0; 
}

// To be completed
template<typename K, typename V, typename Prober, typename Hash, typename KEqual>
HashTable<K,V,Prober,Hash,KEqual>::~HashTable()
{
    for (unsigned int i = 0; i < this->table_.size(); ++i){
        if (this->table_.at(i) != nullptr){
            delete this->table_.at(i);
        }
    }
}

// To be completed
template<typename K, typename V, typename Prober, typename Hash, typename KEqual>
bool HashTable<K,V,Prober,Hash,KEqual>::empty() const
{

    for (unsigned int i = 0; i < this->table_.size(); ++i){
        // No non-deleted key, no pair/item in that location
        if (this->table_.at(i) != nullptr){
            
            if (this->table_.at(i)->deleted == false){
                return false;
            }
        }
    }

    return true; 
}

// To be completed
template<typename K, typename V, typename Prober, typename Hash, typename KEqual>
size_t HashTable<K,V,Prober,Hash,KEqual>::size() const
{
    size_t result = 0; 

    // std::cout << "ITS CALLING" << std::endl;
    for (unsigned int i = 0; i < this->table_.size(); ++i){
        if ( (this->table_.at(i) != nullptr) && (this->table_.at(i)->deleted == false) ){
            ++result; 
        }
    }
        
    return result; 
}

// To be completed
/**
 * @brief Inserts a new item into the map, or, if an item with the
 *        given key already exists, it updates the Value of that item
 *        with the second value of the pair, p
 * 
 * @param p Pair to insert  
 * @throw std::logic_error If no free location can be found
 */
template<typename K, typename V, typename Prober, typename Hash, typename KEqual>
void HashTable<K,V,Prober,Hash,KEqual>::insert(const ItemType& p)
{
    this->currLoadingFactor = static_cast<double>(totalItems) / static_cast<double>(this->table_.size());

    if (currLoadingFactor >= maxLoadingFactor){
        resize();
    }

    // If the given key already exists in the table
    HashItem* findResult = internalFind(p.first); 

    // If given key does not exist 
    if (findResult == nullptr){

        // Create new item
        HashItem* newItem = new HashItem(p);

        // Probe result
        HASH_INDEX_T probeResult = probe(p.first); 

        // If probeResult is npos
        if (probeResult == npos){

            // If loadingFactor is 1
            if (maxLoadingFactor == 1){
                resize();
                
                // Probe again
                probeResult = probe(p.first); 

                if (probeResult == npos){
                    delete newItem; 
                    throw std::logic_error("No free location available xddx"); 
                }

                table_.at(probeResult) = newItem; 
                ++this->currItems;
                ++totalItems;
            }

            else{
                delete newItem;
                throw std::logic_error("No free location available xddx"); 
            }
        }

        // If probeResult is not npos
        else if (probeResult != npos){
            table_.at(probeResult) = newItem;
            ++this->currItems;
            ++totalItems;
        }

        else{
            delete newItem;
        }
    }

    // If given key exists
    else if (findResult != nullptr){
        findResult->item.second = p.second; 
    }
}

// To be completed
/**
 * @brief Removes (marks as deleted) the item with the given key.  
 * Does nothing if an item with the given key does not exist.
 * 
 * @param key 
 */
template<typename K, typename V, typename Prober, typename Hash, typename KEqual>
void HashTable<K,V,Prober,Hash,KEqual>::remove(const KeyType& key)
{
    // find if key exists
    HashItem* findResult = internalFind(key); 

    // If it exists
    if (findResult != nullptr){

        // If it hasn't been deleted
        if (findResult->deleted == false){
            // // should not be nullptr
            // HashItem* wantedItem = internalFind(key); 
            
            // // Personal sanity measure if it happens
            // if (wantedItem == nullptr){
            //     std::cout << "ERROR - WANTED ITEM IS NULL IN REMOVE WHEN IT SHOULD NOT BE" << std::endl; 
            //     return; 
            // }

            // Assign watnedItem as deleted 
            findResult->deleted = true; 

            // Remove one item
            --this->currItems; 

            // Adjust loading factor
            this->currLoadingFactor = static_cast<double>(this->currItems) / static_cast<double>(this->table_.size()); 
        }

    }
}


// Complete
/**
 * @brief Finds an item with the given key and returns a pointer 
 * to the key,value pair
 * 
 * @param key 
 * @return ItemType const* nullptr is returned if the key does not exist
 */
template<typename K, typename V, typename Prober, typename Hash, typename KEqual>
typename HashTable<K,V,Prober,Hash,KEqual>::ItemType const * HashTable<K,V,Prober,Hash,KEqual>::find(const KeyType& key) const
{
    HASH_INDEX_T h = this->probe(key);
    std::cout << "FIND H " << h << std::endl; 
    if((npos == h) || nullptr == table_[h] || table_.at(h)->deleted == true){
        return nullptr;
    }
    return &table_[h]->item;
}

// Complete
template<typename K, typename V, typename Prober, typename Hash, typename KEqual>
typename HashTable<K,V,Prober,Hash,KEqual>::ItemType * HashTable<K,V,Prober,Hash,KEqual>::find(const KeyType& key)
{
    HASH_INDEX_T h = this->probe(key);
    if((npos == h) || nullptr == table_[h] ){
        return nullptr;
    }
    return &table_[h]->item;
}

// Complete

template<typename K, typename V, typename Prober, typename Hash, typename KEqual>
const typename HashTable<K,V,Prober,Hash,KEqual>::ValueType& HashTable<K,V,Prober,Hash,KEqual>::at(const KeyType& key) const
{
    HashItem const * item = this->internalFind(key);
    if(item == nullptr) { throw std::out_of_range("Bad key"); }
    return item->item.second;
}

// Complete
template<typename K, typename V, typename Prober, typename Hash, typename KEqual>
typename HashTable<K,V,Prober,Hash,KEqual>::ValueType& HashTable<K,V,Prober,Hash,KEqual>::at(const KeyType& key)
{
    HashItem * item = this->internalFind(key);
    if(item == nullptr) { throw std::out_of_range("Bad key"); }
    return item->item.second;
}

// Complete
template<typename K, typename V, typename Prober, typename Hash, typename KEqual>
const typename HashTable<K,V,Prober,Hash,KEqual>::ValueType& HashTable<K,V,Prober,Hash,KEqual>::operator[](const KeyType& key) const
{
    return this->at(key);
}

// Complete
template<typename K, typename V, typename Prober, typename Hash, typename KEqual>
typename HashTable<K,V,Prober,Hash,KEqual>::ValueType& HashTable<K,V,Prober,Hash,KEqual>::operator[](const KeyType& key)
{
    return this->at(key);
}

// Complete
/**
 * @brief Helper routine to find a given key
 * 
 * @param key 
 * @return HashItem* returns nullptr if key does not exist
 */
template<typename K, typename V, typename Prober, typename Hash, typename KEqual>
typename HashTable<K,V,Prober,Hash,KEqual>::HashItem* HashTable<K,V,Prober,Hash,KEqual>::internalFind(const KeyType& key) const
{
    
    HASH_INDEX_T h = this->probe(key);
    if((npos == h) || nullptr == table_[h] ){
        return nullptr;
    }
    return table_[h];
}


// To be completed
/**
 * @brief Resizes the hash table replacing the old with a new
 * table of the next prime size given in CAPACITIES.  Must rehash
 * all non-deleted items while freeing all deleted items.
 * 
 * Must run in O(m) where m is the new table size
 * 
 * @throws std::logic_error if no more CAPACITIES exist
 */
template<typename K, typename V, typename Prober, typename Hash, typename KEqual>
void HashTable<K,V,Prober,Hash,KEqual>::resize()
{
    // std::cout << "Resize index: " << mIndex_ << std::endl; 
    // std::cout << "Curr size: " << table_.size() << std::endl;
    // ***
    // Take all items in the old tables and store them somewhere
    if (this->CAPACITIES[this->mIndex_] == 1685759167){
        throw std::logic_error("No free location available"); 
    }

    ++mIndex_; 

    std::vector<HashItem*> existingValues; 
    for (unsigned int i = 0; i < this->table_.size(); ++i){
        existingValues.push_back(table_.at(i));
        table_.at(i) = nullptr;  
    }

    table_.assign(CAPACITIES[mIndex_], nullptr); 

    unsigned int newSize = 0; 
    for (unsigned int i = 0; i < existingValues.size(); ++i){
        if (existingValues.at(i) != nullptr && existingValues.at(i)->deleted == false){
            insert(existingValues.at(i)->item);
            delete existingValues.at(i);  
            ++newSize;
        }

        else if (existingValues.at(i) != nullptr && existingValues.at(i)->deleted == true){
            delete existingValues.at(i); 
        }

        else if (existingValues.at(i) == nullptr){
            continue; 
        }
    }

    currItems = newSize; 
    totalItems = newSize;
    currLoadingFactor = static_cast<double>(currItems) / static_cast<double>(table_.size()); 

    // table_.assign(newsize, nullptr)
    // Iterate over old data vector
    // If it's not nullptr, not deleted, insert
    // If it's not nullptr, deleted, delete, set to nullptr
    // If it's nullptr go to next element
    
}

// Almost complete
/**
 * @brief Performs the probing sequence and returns the index
 * of the table location with the given key or the location where
 * key can be inserted (i.e. the index now contains nullptr) but is
 * available.
 * 
 * @param key 
 * @return returns npos is the key does not exist and
 * no free location is available
 */
template<typename K, typename V, typename Prober, typename Hash, typename KEqual>
HASH_INDEX_T HashTable<K,V,Prober,Hash,KEqual>::probe(const KeyType& key) const
{
    HASH_INDEX_T h = hash_(key) % CAPACITIES[mIndex_];
    prober_.init(h, CAPACITIES[mIndex_], key);

    HASH_INDEX_T loc = prober_.next(); 
    totalProbes_++;
    while(Prober::npos != loc)
    {
        if(nullptr == table_[loc] ) {
            return loc;
        }
        // fill in the condition for this else if statement which should 
        // return 'loc' if the given key exists at this location
        else if(table_.at(loc)->item.first == key && table_.at(loc)->deleted == false) {
            return loc;
        }
        loc = prober_.next();
        totalProbes_++;
    }

    return npos;
}

// Complete
template<typename K, typename V, typename Prober, typename Hash, typename KEqual>
void HashTable<K, V, Prober, Hash, KEqual>::reportAll(std::ostream& out) const
{
	for(HASH_INDEX_T i = 0; i < CAPACITIES[mIndex_]; ++i)
	{
		if(table_[i] != nullptr)
		{
			out << "Bucket " << i << ": " << table_[i]->item.first << " " << table_[i]->item.second << std::endl;
		}
	}
}

#endif