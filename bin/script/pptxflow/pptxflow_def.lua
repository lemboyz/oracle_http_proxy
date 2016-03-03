local M = {}

local column_list = 
[[
tftxcode , tftermid , tfteacct , tfinvno , tfbnussn , tfbnossn , tfodcode , tfcustno , tfintype , tfmccode , tftmccode, tfbncode , trim(to_char(tftxmony,'999999999999.00')) tftxmony , tfcharge , trim(to_char(tfrlmony,'999999999999.00')) tfrlmony , tftxfnco , tfcardno , tfmainat , tfreqtno , tfbackno , tfsdtype , to_char(tfacctdt,'yyyy-mm-dd hh24:mi:ss') tfacctdt, tfauthco , tfcovrat , tfcurren , trim(to_char(tfacmony,'999999999999.00')) tfacmony , tftermno , tflocstu , tfbankstu, tfmerstu , tfbfcode , to_char(tfdate,'yyyy-mm-dd hh24:mi:ss') tfdate, tftxdeta , tftranno , tfacqinsid , tffwdinsid , tfrcvinsid , tftrk2 , tftrk3 , tfmemo , tfmemo1 , tftxfee , tfperdata , tfdebitfee , tfmccode2 , tfextmccode2, to_char(tfmercdt,'yyyy-mm-dd hh24:mi:ss') tfmercdt, tfmemo_2bank , tfcardtype , tfic_tc , tfic_info , tfcdtype , txfwdcode , pinblock_flag, fwdtracenum , fwdrefernum , tfsettstu , tfiaw , tfiac , tfdistcode , tfpfx 
]]

function M.get_column_list()
	return column_list
end

return M
