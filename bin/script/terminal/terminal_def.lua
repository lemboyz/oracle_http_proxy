local M = {}

local column_list = 
[[
terminal_id    ,
status         ,
address        ,
device         ,
type           ,
master         ,
groupid        ,
district       ,
fwdmccode      ,
mach_id        ,
location       ,
term_ver       ,
capk_version   ,
term_ic_version,
insertdatetime ,
dzpzflag       ,
termduty       ,
program_type    
]]

function M.get_column_list()
	return column_list
end

return M
