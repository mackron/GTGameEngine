-- Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

Editor.DataFilesWatcher = {}
Editor.DataFilesWatcher.RegisteredExplorers = {}

function Editor.DataFilesWatcher.RegisterExplorer(explorer)
    Editor.DataFilesWatcher.RegisteredExplorers[#Editor.DataFilesWatcher.RegisteredExplorers + 1] = explorer;
end

function Editor.DataFilesWatcher.DeregisterExplorer(explorer)
    table.remove(GT.table.indexof(Editor.DataFilesWatcher.RegisteredExplorers, explorer));
end

function Editor.DataFilesWatcher.OnInsert(absolutePath)
    for i,value in ipairs(Editor.DataFilesWatcher.RegisteredExplorers) do
        value:OnFileInsert(absolutePath);
    end
end

function Editor.DataFilesWatcher.OnRemove(absolutePath)
    for i,value in ipairs(Editor.DataFilesWatcher.RegisteredExplorers) do
        value:OnFileRemove(absolutePath);
    end
end

function Editor.DataFilesWatcher.OnRename(absolutePath, absolutePathNew)
    for i,value in ipairs(Editor.DataFilesWatcher.RegisteredExplorers) do
        value:OnFileRename(absolutePath, absolutePathNew);
    end
end

function Editor.DataFilesWatcher.OnUpdate(absolutePath)
    for i,value in ipairs(Editor.DataFilesWatcher.RegisteredExplorers) do
        value:OnFileUpdate(absolutePath);
    end
end
