#pragma once

/// <summary>
/// Objet JUANITO_MOVING_AVERAGE : Objet permettant de gérer une moyenne mobile par une queue FIFO.
/// <remark>
/// Basé sur un ring buffer de taille N.
/// La moyenne est recalculée en O(1) à chaque push.
/// @tparam T    Type des éléments stockés(doit supporter + , -et la division)
/// @tparam N    Taille de la fenêtre de moyenne mobile
/// </remark>
/// </summary>
template <typename T, size_t N>
class JUANITO_MOVING_AVERAGE {
public:
    /// <summary>
    /// Constructeur
    /// </summary>
    JUANITO_MOVING_AVERAGE() : _head(0), _tail(0), _count(0), _sum(0) {}

    /// <summary>
    /// Ajoute une nouvelle valeur dans la fenêtre.
    /// <remark>
    /// Si la fenêtre est pleine, la valeur la plus ancienne est remplacée.
    /// </remark>
    /// </summary>
    void Push(const T& value) {
        if (!isnan(value) && !isinf(value))
        {
            if (IsFull())
            {
                _sum -= _buffer[_head];       // Soustrait la valeur sortante
                _head = (_head + 1) % N;
                --_count;
            }
            _buffer[_tail] = value;
            _sum += value;                    // Ajoute la valeur entrante
            _tail = (_tail + 1) % N;
            ++_count;
        }
    }

    /// <summary>
    /// Retourne la moyenne mobile courante.
    /// <remark>
    /// @param out  Référence où stocker la moyenne.
    /// @return true si succès, false si la fenêtre est vide.
    /// </remark>
    /// </summary>
    bool Average(T& out) const
    {
        if (IsEmpty())
        {
            return false;
        }
        out = _sum / static_cast<T>(_count);
        return true;
    }

    /// <summary>
    /// Retire et retourne l'élément en tête (dequeue).
    /// <remark>
    /// @param out  Référence où stocker l'élément retiré.
    /// @return true si succès, false si la file est vide.
    /// </remark>
    /// </summary>
    bool Pop(T& out) {
        if (IsEmpty())
        {
            return false;
        }
        _sum -= _buffer[_head];
        out = _buffer[_head];
        _head = (_head + 1) % N;
        --_count;
        return true;
    }

    /// <summary>
    /// Consulte l'élément en tête sans le retirer (peek).
    /// <remark>
    /// @param out  Référence où stocker l'élément.
    /// @return true si succès, false si la file est vide.
    /// </remark>
    /// </summary>
    bool Peek(T& out) const {
        if (IsEmpty())
        {
            return false;
        }
        out = _buffer[_head];
        return true;
    }

    /// <summary>
    /// Vide la queue et remet la somme à zéro.
    /// </summary>
    void Clear() {
        _head = _tail = _count = 0;
        _sum = 0;
    }

    /// <summary>
    /// True si la liste est vide.
    /// </summary>
    bool   IsEmpty() const { return _count == 0; }
    
    /// <summary>
    /// True si la liste est pleine.
    /// </summary>
    bool   IsFull()  const { return _count == N; }

    /// <summary>
    /// Renvoi le nombre d'élément dans la queue.
    /// </summary>
    size_t Size()    const { return _count; }

    /// <summary>
    /// Renvoi la capacité de la queue.
    /// </summary>
    size_t Capacity()const { return N; }

private:
    T      _buffer[N];                          // Queue d'élément T
    size_t _head;                               // Tête : Pointe sur le prochain élément à lire (le plus ancien)
    size_t _tail;                               // Queue : Pointe sur le prochain emplacement libre ou écrire (le plus récent)
    size_t _count;                              // Nombre d'élément dans la queue
    T      _sum;                                // Somme des éléments dans la queue
};
