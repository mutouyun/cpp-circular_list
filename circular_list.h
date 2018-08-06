#pragma once

#include <cstddef> // size_t
#include <cstdint> // uint8_t
#include <utility> // std::swap, std::forward, std::move
#include <new>     // placement allocation

template <typename T>
class circular_list
{
    uint8_t* buff_;
    size_t   size_;
    size_t   begin_, count_;

public:
    circular_list(const circular_list&) = delete;
    circular_list& operator=(const circular_list&) = delete;

    circular_list(void* buff, size_t size)
        : buff_(static_cast<uint8_t*>(buff))
        , size_(size), begin_(0), count_(0)
    {

    }

    circular_list(circular_list&& rhs)
        : circular_list(nullptr, 0)
    {
        swap(rhs);
    }

    ~circular_list(void)
    {
        clear();
    }

    circular_list& operator=(circular_list rhs)
    {
        swap(rhs);
        return *this;
    }

    void swap(circular_list& rhs)
    {
        std::swap(buff_ , rhs.buff_ );
        std::swap(size_ , rhs.size_ );
        std::swap(begin_, rhs.begin_);
        std::swap(count_, rhs.count_);
    }

    T& at(size_t i)
    {
        return const_cast<T&>(const_cast<const circular_list*>(this)->at(i));
    }

    const T& at(size_t i) const
    {
        return *reinterpret_cast<T*>(buff_ + ((begin_ + i * sizeof(T)) % size_));
    }

          T& operator[](size_t i)       { return at(i); }
    const T& operator[](size_t i) const { return at(i); }

    size_t size     (void) const { return size_ ; }
    size_t count    (void) const { return count_; }
    size_t max_count(void) const { return size_ / sizeof(T); }

    bool null (void) const { return (buff_ == nullptr) || (size_ == 0); }
    bool empty(void) const { return (count_ == 0) || null(); }
    bool full (void) const { return count() >= max_count(); }

    bool pop_front(void)
    {
        if (empty()) return false;
        reinterpret_cast<T*>(buff_ + begin_)->~T();
        begin_ = (begin_ + sizeof(T)) % size_;
        -- count_;
        return true;
    }

    bool pop_back(void)
    {
        if (empty()) return false;
        at(-- count_).~T();
        return true;
    }

    void push_front(const T& val) { emplace_front(val); }
    void push_back (const T& val) { emplace_back (val); }

    class iterator
    {
        friend class circular_list;

        circular_list* list_;
        size_t cursor_;

        iterator(circular_list* list, size_t cursor)
            : list_(list), cursor_(cursor)
        {}

    public:
        iterator(const iterator&) = default;
        iterator& operator=(const iterator&) = default;

        bool operator==(const iterator& rhs) { return (list_ == rhs.list_) && (cursor_ == rhs.cursor_); }
        bool operator!=(const iterator& rhs) { return !operator==(rhs); }

        const iterator operator+(int i) const { return { list_, cursor_ + i }; }
        const iterator operator-(int i) const { return { list_, cursor_ - i }; }

        iterator& operator+=(int i) { cursor_ += i; return *this; }
        iterator& operator-=(int i) { cursor_ -= i; return *this; }

        iterator& operator++(void) { ++ cursor_; return *this; }
        iterator& operator--(void) { -- cursor_; return *this; }

        const iterator operator++(int) { iterator tmp; ++ tmp; return tmp; }
        const iterator operator--(int) { iterator tmp; -- tmp; return tmp; }

        const T* operator->(void) const { return &(list_->at(cursor_)); }
              T* operator->(void)       { return &(list_->at(cursor_)); }

        const T& operator*(void) const { return list_->at(cursor_); }
              T& operator*(void)       { return list_->at(cursor_); }
    };

    friend class iterator;
    typedef const iterator const_iterator;

    const_iterator  begin(void) const { return { const_cast<circular_list*>(this), 0 }; }
          iterator  begin(void)       { return { this, 0 }; }
    const_iterator cbegin(void) const { return begin();     }

    const_iterator  end(void) const { return { const_cast<circular_list*>(this), count_ }; }
          iterator  end(void)       { return { this, count_ }; }
    const_iterator cend(void) const { return end();            }

    const T& front(void) const { return at(0); }
          T& front(void)       { return at(0); }

    const T& back(void) const { return at(count_ - 1); }
          T& back(void)       { return at(count_ - 1); }

    circular_list& operator<<(const T& val)
    {
        emplace_back(val);
        return *this;
    }

    circular_list& operator<<(T&& val)
    {
        emplace_back(std::move(val));
        return *this;
    }

    circular_list& operator<<(const circular_list& list)
    {
        for (const T& val : list) emplace_back(val);
        return *this;
    }

    circular_list& operator<<(circular_list&& list)
    {
        for (T& val : list) emplace_back(std::move(val));
        return *this;
    }

    template <typename ... U>
    void emplace_front(U&&... args)
    {
        if (full()) pop_back();
        begin_ = (size_ + begin_ - sizeof(T)) % size_;
        ::new(buff_ + begin_) T(std::forward<U>(args)...);
        ++ count_;
    }

    template <typename ... U>
    void emplace_back(U&&... args)
    {
        if (full()) pop_front();
        ::new(&at(count_)) T(std::forward<U>(args)...);
        ++ count_;
    }

    void clear(void)
    {
        while (!empty()) pop_back();
    }

    void reset(void* buff, size_t size)
    {
        clear();
        buff_  = buff;
        size_  = size;
        begin_ = count_ = 0;
    }
};
