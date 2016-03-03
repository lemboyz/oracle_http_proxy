package.path = "./script/terminal/?.lua;"..package.path
package.cpath = "./script/?.so;"..package.cpath

function MAIN()
	body = httpGetRequestBody()
	log("body:"..body)

	local params = parseUriParam(body)
	
	local termid = params.termid

	local terminal_def = require("terminal_def")
	local column_list = terminal_def.get_column_list()
	local sql = "select "..column_list.." from ybsqz.terminal where terminal_id=:v1"
	local rs = sql_select(sql, termid)
	local ret = {}
	if rs[1] then
		ret = rs[1]
	end

	local cjson = require("cjson")
	httpSetResponseBody(cjson.encode(ret))
end
