#ifndef PERSISTENTMAP_H
#define PERSISTENTMAP_H

#include "PersistentCache.h"

template<typename KeyT, typename ValueT>
class PersistentMap : private PersistentCache
{
public:
    typedef KeyT Key;
    typedef ValueT Value;
    typedef QMap<Key, Value> Map;

private:
    template<typename T>
    struct MapOperator
    {
        Map all() const
        {
            return map.read<Map>(keys);
        }

        Value get(Key const &key) const
        {
            return all()[key];
        }

        void overwrite(Map const &cache)
        {
            map.write(cache, keys);
        }

        void insert(Key const &key, Value const &value)
        {
            Map cache = all();
            cache.insert(key, value);
            overwrite(cache);
        }


        MapOperator(T map, QStringList keys)
            : map(map)
            , keys(keys)
        {
        }

        T map;
        QStringList keys;
    };

    typedef MapOperator<PersistentMap &> Operator;
    typedef MapOperator<PersistentMap const &> ConstOperator;

public:
    PersistentMap(QString name, QDir baseDir)
        : PersistentCache(name, baseDir)
    {
    }

    Operator operator() (QString key1)
    {
        return Operator(*this, QStringList() << key1);
    }

    ConstOperator operator() (QString key1) const
    {
        return ConstOperator(*this, QStringList() << key1);
    }

    Operator operator() (QString key1, QString key2)
    {
        return Operator(*this, QStringList() << key1 << key2);
    }

    ConstOperator operator() (QString key1, QString key2) const
    {
        return ConstOperator(*this, QStringList() << key1 << key2);
    }
};

#endif // PERSISTENTMAP_H
