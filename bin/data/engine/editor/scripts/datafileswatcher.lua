-- Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

Editor.DataFilesWatcher = {}
Editor.DataFilesWatcher.RegisteredExplorers = {}

function Editor.DataFilesWatcher.RegisterExplorer(explorer)
    Editor.DataFilesWatcher.RegisteredExplorers[#Editor.DataFilesWatcher.RegisteredExplorers + 1] = explorer;
end

function Editor.DataFilesWatcher.DeregisterExplorer(explorer)
    table.remove(table.indexof(Editor.DataFilesWatcher.RegisteredExplorers, explorer));
end

function Editor.DataFilesWatcher.OnInsert(fileInfo)
    for i,value in ipairs(Editor.DataFilesWatcher.RegisteredExplorers) do
        value:OnFileInsert(fileInfo);
    end
end

function Editor.DataFilesWatcher.OnRemove(fileInfo)
    for i,value in ipairs(Editor.DataFilesWatcher.RegisteredExplorers) do
        value:OnFileRemove(fileInfo);
    end
end

function Editor.DataFilesWatcher.OnUpdate(fileInfo)
    for i,value in ipairs(Editor.DataFilesWatcher.RegisteredExplorers) do
        value:OnFileUpdate(fileInfo);
    end
end