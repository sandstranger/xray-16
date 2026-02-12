///////////////////////////////////////////////////////////////
// Scope.h
// Scope - апгрейд оружия снайперский прицел
///////////////////////////////////////////////////////////////

#pragma once

#include "inventory_item_object.h"

class CScope final : public CInventoryItemObject
{
protected:
    typedef CInventoryItemObject inherited;

private:
    DECLARE_SCRIPT_REGISTER_FUNCTION(CGameObject);
};
