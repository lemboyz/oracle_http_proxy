package.path = "./script/?.lua;"..package.path
package.cpath = "./script/?.so;"..package.cpath

function MAIN()
	body = httpGetRequestBody()
	log("body:"..body)

	local params = parseUriParam(body)
	
	local tftxcode = params.tftxcode

	local pptxflow_def = require("pptxflow_def")
	local column_list = pptxflow_def.get_column_list()
	local sql = "select "..column_list.." from pptxflow where tftxcode=:v1"
	local rs = sql_select(sql, tftxcode)
	--local rs = {[1]={tftxcode='111',tfmccode='123456'}}
	local ret = {}
	if rs[1] then
		ret = rs[1]
		--for k,v in pairs(ret) do
		--	log(k.." = "..v)
		--end
	end

	local cjson = require("cjson")

	httpSetResponseBody(cjson.encode(ret))
end
