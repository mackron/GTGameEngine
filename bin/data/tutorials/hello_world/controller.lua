--[[public:number]] self.MoveSpeed = 5.0;

function self:OnUpdate(deltaTimeInSeconds)
    local newPosition = self:GetPosition();

    if Game.IsKeyDown(GTEngine.Keys.ArrowUp) then
        newPosition.z = newPosition.z - self.MoveSpeed * deltaTimeInSeconds;
    end

    if Game.IsKeyDown(GTEngine.Keys.ArrowDown) then
        newPosition.z = newPosition.z + self.MoveSpeed * deltaTimeInSeconds;
    end

    if Game.IsKeyDown(GTEngine.Keys.ArrowLeft) then
        newPosition.x = newPosition.x - self.MoveSpeed * deltaTimeInSeconds;
    end

    if Game.IsKeyDown(GTEngine.Keys.ArrowRight) then
        newPosition.x = newPosition.x + self.MoveSpeed * deltaTimeInSeconds;
    end


    self:SetPosition(newPosition);
end