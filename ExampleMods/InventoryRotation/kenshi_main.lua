print('Inventory Rotation Mod Loading')

-- Listens to the 'R' key when holding an item in an inventory view, if pressed the item rotates 90 degrees

-- TODO:
--  - Match shadow to draggable (hopefully fixes selecting jumping to inside the original bounds)
--  - Save and load

local guiTileSize = 25
local selectedItemIcon, selectedItemSerial;

function MatchInventoryIconToItemSize(inventoryIcon, selectedItem)
    if (inventoryIcon.widget:getWidth() ~= selectedItem.itemWidth * guiTileSize) then
        local wSlots = selectedItem.itemWidth
        local hSlots = selectedItem.itemHeight
        local pixelWidth = wSlots * guiTileSize
        local pixelHeight = hSlots * guiTileSize

        inventoryIcon.widget:setSize(pixelWidth, pixelHeight)

        local iconWidget = inventoryIcon.widget:getChildAt(1):getChildAt(0)
        iconWidget:setRealSize(1, 1)

        local rotatingSkin = MyGUI.RotatingSkin.FromWidget(iconWidget)

        rotatingSkin:setCenter(math.floor(iconWidget:getHeight() / 2), math.floor(iconWidget:getWidth() / 2))
        rotatingSkin:setAngle(1.5707) -- Half PI, aka 90 deg

        -- Swap the size (aspect ratio)
        local rotatedW = hSlots / wSlots
        local rotatedH = wSlots / hSlots
        iconWidget:setRealSize(rotatedW, rotatedH)

        -- Re-center it inside the parent
        iconWidget:setRealPosition((1 - rotatedW) / 2, (1 - rotatedH) / 2)
    end
end

function ukn_createDraggableItem_postfix(_th, item, gameData_maybe, widgetParent)
    local thisItemIcon = Kenshi.InventoryIcon.CastTo(_th)
    MatchInventoryIconToItemSize(thisItemIcon, item)

    -- If widgetParent is nil, we are dragging the item
    if widgetParent == nil then
        print("Dragging " .. thisItemIcon.inventoryItem.displayName .. " started")
        selectedItemIcon = thisItemIcon
        selectedItemSerial = thisItemIcon.inventoryItem.handle.serial
    elseif selectedItemIcon ~= nil then
        -- TODO: ID check to make sure it's the same item. Can't do that until we find a unique id
        print("Dragging " .. thisItemIcon.inventoryItem.displayName .. " ended")
        if selectedItemSerial == thisItemIcon.inventoryItem.handle.serial then
            print("All dragging ended, clearing reference")
            selectedItemIcon = nil
            selectedItemSerial = 0
        end
    end
end

function InputHandler_keyDownEvent(_, keyCode)
    local keyName = OISKeyCode[keyCode] or ("Unknown(" .. tostring(keyCode) .. ")")
    if keyName == "KC_R" and selectedItemIcon ~= nil then
        local selectedItem = selectedItemIcon.inventoryItem
        print("Holding and rotating: " .. selectedItem.displayName)
        selectedItem.itemWidth, selectedItem.itemHeight = selectedItem.itemHeight, selectedItem.itemWidth
        MatchInventoryIconToItemSize(selectedItemIcon, selectedItem)
    end

    if keyName == "KC_O" then
        print("Held item is: " .. selectedItemSerial)
    end
end
