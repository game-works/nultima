#ifndef __INVENTORY_H__
#define __INVENTORY_H__

#include <vector>
#include "Item.h"

class Inventory
{
public:
    Inventory();

    void addItem(Item*);
    void removeItem(int id);

    int getTotalWeight();

private:
    std::vector<Item*> m_items;

};


#endif
