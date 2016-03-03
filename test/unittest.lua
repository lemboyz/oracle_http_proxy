#!/usr/local/bin/lua

local cjson = require("cjson")


function echoRed(str)
    echoColor(31, str)
end
function echoGreen(str)
    echoColor(32, str)
end
function echoColor(num, str)
    -- 30 black
    -- 31 red
    -- 32 green
    -- 33 yellow
    -- 34 blue
    -- 35 purple
    -- 36 light blue
    -- other: white 
    print('\27[' .. num .. 'm' .. str .. '\27[m')
end

function decodeURI(s)
    s = string.gsub(s, '%%(%x%x)', function(h) return string.char(tonumber(h, 16)) end)
    return s
end

function encodeURI(s)
    s = string.gsub(s, "([^%w%.%- ])", function(c) return string.format("%%%02X", string.byte(c)) end)
    return string.gsub(s, " ", "+")
end

function popenRun(cmd)
    local ret = ""
    local t = io.popen(cmd)
    ret = t:read("*all");
    t:close()
    return ret;
end

function trim(s)
  return (s:gsub("^%s*(.-)%s*$", "%1"))
end

function removeWhitespace(str)
	if not str then
		return str
	end

	local ret = "";
	for i=1,#str do
		if string.sub(str, i, i) ~= " " then
			ret = ret .. string.sub(str, i, i)
		end
	end

	return ret
end

function unittest(ipport, url, postdata)
    local url = string.format("http://%s%s",ipport, url)
    local cmd = "curl -s '" .. url .. "'" 
	if postdata and #postdata>0 then
		cmd = cmd .. " -d '" .. postdata .. "'"
	end
    print(cmd)
    local ret = popenRun(cmd)
    ret = decodeURI(ret)
	print(ret)

	ret = removeWhitespace(ret)	
    return ret
end

--curl 'http://0.0.0.0:15210/pptxflow/querybytftxcode' -d'tftxcode=2016012110309942'
function test1()
	local ipport = "0.0.0.0:15210"
	local url = "/pptxflow/querybytftxcode"
	local postdata = "tftxcode=2016012110309945"
	local resp = unittest(ipport, url, postdata)
	local rs = cjson.decode(resp)
	if rs and rs.tftxcode=='2016012110309945'  then
		--for k,v in pairs(rs) do
		--	print(k.."="..v)
		--end
		echoGreen("TEST OK")
	else
		echoRed("TEST FAIL.")
	end
end

function test2()
	local ipport = "0.0.0.0:15210"
	local url = "/terminal/querybytermid"
	local postdata = "termid=16100101"
	local resp = unittest(ipport, url, postdata)
	local rs = cjson.decode(resp)
	if rs and rs.terminal_id == "16100101" then
		echoGreen("TEST OK")
	else
		echoRed("TEST FAIL.")
	end
end

function main()

	test1()
	test2()
	
end

main()

