function makeSVG(tag, attrs) {
  const el = document.createElementNS("http://www.w3.org/2000/svg", tag)
  for (let k in attrs) el.setAttribute(k, attrs[k])
  return el
}
//----------------------------------------------------------------------------
function loadSVG(id, name) {
  $("#" + id).load(
    "static/SLD/" + name + ".svg",
    function (response, status, xhr) {
      if (status == "error") {
        $("#" + id).load("static/SLD/fail.svg")
      } else {
        console.log("svg load success")
      }
      count = count + 1
    }
  )
}
//----------------------------------------------------------------------------
const getSVGIDList = async () => {
  const nodeList = Array.apply(null, document.querySelectorAll("g[data-eid]"))
  // console.log("SSSSSSS");
  nodeList.forEach((object) => {
    const ID = $(object).attr("data-eid")
    const col = $(object).attr("data-col")
    const datatype = $(object).attr("data-datatype")
    if (ID !== undefined && col !== undefined) {
      const index = searchList.findIndex((search) => search.ID === ID)
      if (index !== -1) {
        if (datatype !== undefined) {
          searchList[index]["project"][datatype] = 1
        }
      } else {
        search = { project: {} }
        search.ID = ID
        search.col = col
        if (datatype !== undefined) {
          search.project[datatype] = 1
        }
        searchList.push(search)
      }
    }
  })
  // console.log(searchList);
}
//----------------------------------------------------------------------------
const getSearchListData = function (e) {
  $.getJSON(
    $SCRIPT_ROOT + "/getSearchListData",
    {
      // first:e,
      searchList: JSON.stringify(searchList),
    },
    function (data) {
      // console.log(data)
      const nodeList = Array.apply(
        null,
        document.querySelectorAll("g[data-eid]")
      )
      const allData = data.data
      const equipData = data.equipData
      nodeList.forEach((node) => {
        const ID = $(node).attr("data-eid")
        const datatype = $(node).attr("data-datatype")
        const bit = $(node).attr("data-bit")
        const colorSet = $(node).attr("data-color")
        const switchSet = $(node).attr("data-switch")
        const bit_pos = $(node).attr("data-bit-pos") //代表需先解析資料的第幾個點位
        const inverse = $(node).attr("data-inverse") //代表須不須反轉數值 ie. 1->0; 0->1
        const fuseSet = $(node).attr("data-fuse")

        if (datatype === undefined) return false
        // ------------------------Text--------------------------
        //number
        const config = {
          ID,
          datatype,
          bit,
          value: allData[ID] !== undefined ? allData[ID][datatype] : undefined,
          set:
            equipData[ID] !== undefined
              ? equipData[ID].tags.find((tag) => tag.tag === datatype)
              : {},
        }
        if (colorSet != undefined) {
          // color
        }
        if (switchSet !== undefined) {
          // switch
        } else if (fuseSet !== undefined) {
          //fuse
        } else {
          // text
          // number
          if (bit === undefined) {
            svgText(config)
            svgText(
              ID,
              datatype,
              allData[ID] !== undefined ? allData[ID][datatype] : undefined,
              equipData[ID] !== undefined
                ? equipData[ID].tags.find((tag) => tag.tag === datatype)
                : {}
            )
          }
          // Array
          else {
            svgArrayText(config)
            svgArrayText(
              ID,
              datatype,
              bit,
              allData[ID] !== undefined ? allData[ID][datatype] : undefined,
              equipData[ID] !== undefined
                ? equipData[ID].tags.find((tag) => tag.tag === datatype)
                : {}
            )
          }
        }

        // ------------------------Color--------------------------
        if (
          colorSet !== undefined &&
          bit === undefined &&
          bit_pos === undefined
        ) {
          let inv = inverse !== undefined ? parseInt(inverse) : 0
          if (inv === 1 && allData[ID] !== undefined) {
            allData[ID][datatype] = inverse_val(allData[ID][datatype])
          }
          svgColorSet(
            ID,
            datatype,
            allData[ID] !== undefined ? allData[ID][datatype] : undefined,
            colorSet
          )
        } else if (
          colorSet !== undefined &&
          bit !== undefined &&
          bit_pos === undefined
        ) {
          // console.log(allData[ID][datatype][bit])
          // console.log(allData[ID][datatype])
          let inv = inverse !== undefined ? parseInt(inverse) : 0
          if (inv === 1 && allData[ID] !== undefined) {
            allData[ID][datatype][bit] = inverse_val(allData[ID][datatype][bit])
          }
          //if(allData[ID][datatype]!==undefined)
          svgColorSet(
            ID,
            datatype,
            allData[ID] !== undefined ? allData[ID][datatype][bit] : undefined,
            colorSet
          )
        } else if (
          colorSet !== undefined &&
          bit === undefined &&
          bit_pos !== undefined
        ) {
          // console.log(allData[ID][datatype][bit])
          let bit_val = undefined
          let inv = inverse !== undefined ? parseInt(inverse) : 0
          bit_val = bitN_of_num(allData[ID][datatype], bit_pos)
          if (inv === 1) {
            bit_val = inverse_val(bit_val)
          }

          svgColorSet(
            ID,
            datatype,
            bit_val !== undefined ? bit_val : undefined,
            colorSet,
            bit_pos
          )
        }
        // ------------------------svgSwitch--------------------------
        if (
          switchSet !== undefined &&
          bit === undefined &&
          bit_pos === undefined
        ) {
          // console.log("WWW",allData[ID][datatype],"SSSqq")
          let inv = inverse !== undefined ? parseInt(inverse) : 0
          if (inv === 1 && allData[ID] !== undefined) {
            allData[ID][datatype] = inverse_val(allData[ID][datatype])
          }
          svgSwitchSet(
            ID,
            datatype,
            allData[ID] !== undefined ? allData[ID][datatype] : undefined,
            switchSet
          )
        } else if (
          switchSet !== undefined &&
          bit !== undefined &&
          bit_pos === undefined
        ) {
          // console.log("WWW", allData[ID][datatype][bit],"SSSqq")
          let inv = inverse !== undefined ? parseInt(inverse) : 0
          if (inv === 1 && allData[ID] !== undefined) {
            allData[ID][datatype][bit] = inverse_val(allData[ID][datatype][bit])
          }

          if (allData[ID][datatype] !== undefined) {
            svgSwitchSet(
              ID,
              datatype,
              allData[ID] !== undefined
                ? allData[ID][datatype][bit]
                : undefined,
              switchSet,
              bit
            )
          }
        } else if (
          switchSet !== undefined &&
          bit === undefined &&
          bit_pos !== undefined
        ) {
          let bit_val = undefined
          let inv = inverse !== undefined ? parseInt(inverse) : 0
          bit_val = bitN_of_num(allData[ID][datatype], bit_pos)
          if (inv === 1) {
            bit_val = inverse_val(bit_val)
          }

          svgSwitchSet(
            ID,
            datatype,
            bit_val !== undefined ? bit_val : undefined,
            switchSet,
            bit,
            bit_pos
          )
        }
        // ------------------------svgFuse--------------------------
        if (
          fuseSet !== undefined &&
          bit === undefined &&
          bit_pos === undefined
        ) {
          // console.log("WWW",allData[ID][datatype],"SSSqq")
          let inv = inverse !== undefined ? parseInt(inverse) : 0
          if (inv === 1 && allData[ID] !== undefined) {
            allData[ID][datatype] = inverse_val(allData[ID][datatype])
          }

          svgFuseSet(
            ID,
            datatype,
            allData[ID] !== undefined ? allData[ID][datatype] : undefined,
            fuseSet
          )
        } else if (
          fuseSet !== undefined &&
          bit !== undefined &&
          bit_pos === undefined
        ) {
          // console.log("WWW", allData[ID][datatype][bit],"SSSqq")
          let inv = inverse !== undefined ? parseInt(inverse) : 0
          if (inv === 1 && allData[ID] !== undefined) {
            allData[ID][datatype][bit] = inverse_val(allData[ID][datatype][bit])
          }

          if (allData[ID][datatype] !== undefined) {
            svgFuseSet(
              ID,
              datatype,
              allData[ID] !== undefined
                ? allData[ID][datatype][bit]
                : undefined,
              fuseSet,
              bit
            )
          }
        } else if (
          fuseSet !== undefined &&
          bit === undefined &&
          bit_pos !== undefined
        ) {
          let bit_val = undefined
          let inv = inverse !== undefined ? parseInt(inverse) : 0
          bit_val = bitN_of_num(allData[ID][datatype], bit_pos)
          if (inv === 1) {
            bit_val = inverse_val(bit_val)
          }

          svgFuseSet(
            ID,
            datatype,
            bit_val !== undefined ? bit_val : undefined,
            fuseSet,
            bit,
            bit_pos
          )
        }
      })
    }
  )
  return false
}
//----------------------------------------------------------------------------
function bitN_of_num(num, bit_pos) {
  const bit_mask = Math.pow(2, bit_pos)
  return num !== undefined ? (num & bit_mask) / bit_mask : undefined
}
//----------------------------------------------------------------------------
function inverse_val(val) {
  return val !== undefined ? (val ? 0 : 1) : undefined
}
//----------------------------------------------------------------------------
function round(value, num) {
  return value.toFixed(num)
}
//----------------------------------------------------------------------------
function translate(value, translate, error) {
  if (translate !== null) {
    return translate[value] !== undefined
      ? translate[value]
      : error !== undefined
      ? error
      : "---"
  } else {
    return error !== undefined ? error : "---"
  }
}
//----------------------------------------------------------------------------
function translateColor(value, color) {
  if (color !== null) {
    return color[value] !== undefined ? color[value] : "black"
  } else {
    return "black"
  }
}
//----------------------------------------------------------------------------
function bitTranslate(value, set, error) {
  return ((value >> set.index) & 1) === 1
    ? translate(1, set.translate, error)
    : translate(0, set.translate, error)
}
//----------------------------------------------------------------------------
function bitTranslateColor(value, set) {
  return translateColor((value >> set.index) & 1, set.color)
}
//----------------------------------------------------------------------------
function decimalToHexString(number) {
  if (number < 0) {
    number = 0xffffffff + number + 1
  }
  return "0x" + number.toString(16).toUpperCase().padStart(4, "0")
}
//----------------------------------------------------------------------------
function svgText(config) {
  console.log(config)
  let { ID, datatype, bit, value, set } = config
  if (value === undefined) return
  else {
    const objList = [`div div div`, `b`, `font`]
    for (let i in objList) {
      // console.log(ID,datatype,set)
      let obj = $(`g[data-eid=${ID}][data-datatype=${datatype}]`).find(
        objList[i]
      )
      if (obj.length > 0) {
        if (datatype === "control_state2") {
          console.log(value, set.color)
          obj.css({ color: translateColor(value, set.color) })
        }
        if (set.tagType === "number") {
          // console.log("number");
          value = round(value, set.round) //num4math
        } else if (set.tagType === "text") {
          // console.log("text");
          value = translate(value, set.translate, set.error) //num2string
        } else if (ID === "system" && datatype === "status") {
          if (value === 1) value = "運轉"
          else value = "停機"
        }
        obj.text(value)
        break
      } else {
        // console.log("dont find");
      }
    }
  }
}
//----------------------------------------------------------------------------
function svgArrayText(config) {
  let { ID, datatype, bit, value, set } = config
  if (value === undefined) return
  else {
    // console.log(value,ID, datatype,bit,value[bit])
    bit = bit * 1
    const objList = [`div div div`, `b`, `font`]
    for (let i in objList) {
      let obj = $(
        `g[data-eid=${ID}][data-datatype=${datatype}][data-bit=${bit}]`
      )
      let obj_text = obj.find(objList[i])
      if (obj_text.length > 0) {
        if (set.tagType === "numberArray") {
          value = round(value[bit], set.round) //num4math
        } else if (set.tagType === "textArray") {
          value = translate(value[bit], set.translate, set.error) //num2string
        }
        $(obj_text).text(value)
        if (obj.attr("data-datatype") === "soc_string") {
          let battery = obj.filter(`[data-rect='1']`).find("rect")
          if (battery.attr("full_H") === undefined)
            battery.attr("full_H", battery.attr("height"))
          if (battery.length > 0)
            // battery.attr('height', battery.attr('height')*1*95/100)
            battery.attr("height", (battery.attr("full_H") * 1 * value) / 100)
          // soc = battery.attr('height')*1/50*100
          let soc = value
          // console.log(soc)
          if (soc > 60) battery.attr("fill", "#cdeb8b")
          else if (soc < 30) battery.attr("fill", "rgb(248, 84, 84)")
          else battery.attr("fill", "#f2e200")
        }
        break
      }
    }
  }
}
//----------------------------------------------------------------------------
function svgColorSet(ID, datatype, value, colorSet, bit_pos = undefined) {
  // console.log(datatype, bit_pos)
  const color_table = {
    halt_normal: "#00CC66",
    run: "#F85454",
    offline: "#B3B3B3",
    warning: "#F2E200",
    fault: "#FF8000",
  }
  if (datatype === "di1_status_bank" && parseInt(bit_pos) === 1) {
    //mbms fuse
    color = {
      undefined: color_table["offline"],
      0: color_table["fault"],
      1: color_table["run"],
    }
  } else if (datatype === "status_pcs") {
    //pcs_status
    color = {
      undefined: color_table["offline"],
      0: color_table["halt_normal"],
      1: color_table["run"],
      2: color_table["fault"],
      3: color_table["run"],
      4: color_table["run"],
      5: color_table["run"],
      6: color_table["run"],
    }
  } else if (datatype === "mode_string") {
    color = {
      undefined: color_table["offline"],
      2: color_table["halt_normal"],
      3: color_table["run"],
      0: color_table["fault"],
    }
  } else {
    color = {
      undefined: color_table["offline"],
      0: color_table["halt_normal"],
      1: color_table["run"],
      2: color_table["fault"],
    }
  }

  //--填入選擇器條件
  let element_select_condition = `g[data-eid=${ID}][data-datatype=${datatype}][data-color=${colorSet}]`
  if (bit_pos !== undefined) {
    element_select_condition += `[data-bit-pos=${bit_pos}]`
  }
  //--找到元素並修改顏色
  for (let i = 0; i < $(element_select_condition).length; i++) {
    $(element_select_condition).eq(i).children().attr("fill", color[value])
  }
}
//----------------------------------------------------------------------------
let dict_element_referP = {}
let dict_element_offset = {}

function cal_reference(ID, datatype, element_type) {
  let key = ID + "-" + datatype + "_" + element_type
  let origin = undefined
  let temp, reference
  let element_id
  dict_element_referP[key] = {}
  switch (element_type) {
    case "switch1":
      for (
        let i = 0;
        i <
        $(`g[data-eid=${ID}][data-datatype=${datatype}][data-switch=1]`).length;
        i++
      ) {
        //--計算中心點
        origin = $(
          `g[data-eid=${ID}][data-datatype=${datatype}][data-switch=1]`
        )
          .eq(i)
          .children()
          .attr("d")
        element_id = $(
          `g[data-eid=${ID}][data-datatype=${datatype}][data-switch=1]`
        )
          .eq(i)
          .attr("id")
        temp = origin.split(" ")
        reference = [
          parseFloat(temp[1]), //x_reference
          (parseFloat(temp[2]) + parseFloat(temp[5])) / 2,
        ] //y_reference
        dict_element_referP[key][element_id] = reference
      }
      break
    case "fuse1":
      for (
        let i = 0;
        i <
        $(`g[data-eid=${ID}][data-datatype=${datatype}][data-fuse=1]`).length;
        i++
      ) {
        //--計算中心點
        origin = $(`g[data-eid=${ID}][data-datatype=${datatype}][data-fuse=1]`)
          .eq(i)
          .children()
          .attr("d")
        element_id = $(
          `g[data-eid=${ID}][data-datatype=${datatype}][data-fuse=1]`
        )
          .eq(i)
          .attr("id")
        temp = origin.split(" ")
        reference = [
          parseFloat(temp[1]), //x_reference
          parseFloat(temp[2]),
        ] //y_reference
        dict_element_referP[key][element_id] = reference
      }
      break
    default:
      break
  }
}

function cal_offset(ID, datatype, element_type) {
  let key = ID + "-" + datatype + "_" + element_type
  dict_element_offset[key] = {}
  let origin = undefined
  let temp, reference, offset
  switch (element_type) {
    case "switch1":
      //--計算圖形offset
      origin = $(`g[data-eid=${ID}][data-datatype=${datatype}][data-switch=1]`)
        .eq(0)
        .children()
        .attr("d")
      element_id = $(
        `g[data-eid=${ID}][data-datatype=${datatype}][data-switch=1]`
      )
        .eq(0)
        .attr("id")
      //----value=0
      temp = origin.split(" ")
      reference = dict_element_referP[key][element_id]
      offset = [
        parseFloat(temp[1]) - reference[0], //x1 - x_reference
        parseFloat(temp[2]) - reference[1], //y1 - y_reference
        parseFloat(temp[4]) - reference[0], //x2 - x_reference
        parseFloat(temp[5]) - reference[1],
      ] //y2 - y_reference
      dict_element_offset[key]["0"] = offset
      //----value=1
      dict_element_offset[key]["1"] = [
        offset[0],
        offset[1],
        offset[0],
        offset[3],
      ]
      // console.log(dict_element_offset);
      break
    case "fuse1":
      origin = $(`g[data-eid=${ID}][data-datatype=${datatype}][data-fuse=1]`)
        .eq(0)
        .children()
        .attr("d")
      element_id = $(
        `g[data-eid=${ID}][data-datatype=${datatype}][data-fuse=1]`
      )
        .eq(0)
        .attr("id")
      temp = origin.split(" ")
      reference = dict_element_referP[key][element_id]
      offset = []
      let char_node = 0
      for (let i = 0; i < temp.length; i++) {
        if (isNaN(parseFloat(temp[i]))) {
          //該點為'M'或'L'或'C'
          char_node = i
        } else {
          switch ((i - char_node) % 2) {
            case 0: //y點位
              offset.push(parseFloat(temp[i]) - reference[1])
              //console.log(parseFloat(temp[i]), "-", reference[1],"=", offset[offset.length-1]);
              break
            case 1: //x點位
              offset.push(parseFloat(temp[i]) - reference[0])
              //console.log(parseFloat(temp[i]), "-", reference[0],"=", offset[offset.length-1]);
              break
            default:
              break
          }
        }
      }
      dict_element_offset[key]["1"] = offset //1是保險絲連接；0是熔斷
      break
    default:
      break
  }
  console.log(dict_element_referP, dict_element_offset)
}
//----------------------------------------------------------------------------
function svgSwitchSet(
  ID,
  datatype,
  value,
  switchSet,
  bit = undefined,
  bit_pos = undefined
) {
  // console.log(ID, datatype, bit, value, bit, bit_pos)
  if (value === undefined) return

  if (switchSet === "1") {
    // if($(`g[data-eid=${ID}][data-datatype=${datatype}][data-switch=1] path`).attr("originald")===undefined){
    //   let d = $(`g[data-eid=${ID}][data-datatype=${datatype}][data-switch=1] path`).attr("d")
    //   $(`g[data-eid=${ID}][data-datatype=${datatype}][data-switch=1] path`).attr("originald",d)
    // }

    //--初始化時須計算中心參考點、圖形offset座標
    let element_type = "switch1"
    let key = ID + "-" + datatype + "_" + element_type
    if (dict_element_referP[key] === undefined) {
      cal_reference(ID, datatype, element_type)
    }
    if (dict_element_offset[key] === undefined) {
      cal_offset(ID, datatype, element_type)
    }

    //--填入選擇器條件
    let element_select_condition = `g[data-eid=${ID}][data-datatype=${datatype}][data-switch=1]`
    if (bit !== undefined) {
      element_select_condition += `[data-bit=${bit}]`
    }
    if (bit_pos !== undefined) {
      element_select_condition += `[data-bit-pos=${bit_pos}]`
    }

    if (value === 0 || value === 1) {
      for (let i = 0; i < $(element_select_condition).length; i++) {
        let element_id = $(element_select_condition).eq(i).attr("id")
        let reference = dict_element_referP[key][element_id]
        let offset = dict_element_offset[key][value]
        // console.log(reference, offset)
        let nextd = [
          "M",
          offset[0] + reference[0],
          offset[1] + reference[1],
          "L",
          offset[2] + reference[0],
          offset[3] + reference[1],
        ]
        nextd = nextd.join(" ")
        // console.log("nextd", nextd);
        $("#" + element_id)
          .children()
          .attr("d", nextd)
      }
    } else if (value === 2) {
      for (let i = 0; i < $(element_select_condition).length; i++) {
        let element_id = $(element_select_condition).eq(i).attr("id")
        $("#" + element_id)
          .children()
          .attr("d", "")
      }
    }
  }
  //未完成↓↓
  else {
    // console.log($(`g[data-eid=${ID}][data-datatype=${datatype}][data-switch=2][data-bit=${bit}] path`))
    if (
      $(
        `g[data-eid=${ID}][data-datatype=${datatype}][data-switch=2][data-bit=${bit}] path`
      ).attr("originald") === undefined
    ) {
      let d = $(
        `g[data-eid=${ID}][data-datatype=${datatype}][data-switch=2][data-bit=${bit}] path`
      ).attr("d")
      $(
        `g[data-eid=${ID}][data-datatype=${datatype}][data-switch=2][data-bit=${bit}] path`
      ).attr("originald", d)
    }
    const originald = $(
      `g[data-eid=${ID}][data-datatype=${datatype}][data-switch=2][data-bit=${bit}] path`
    ).attr("originald")
    // console.log(bit, originald)
    if (value === 2) {
      // console.log(ID,datatype)
      $(
        `g[data-eid=${ID}][data-datatype=${datatype}][data-switch=2][data-bit=${bit}] path`
      ).attr("d", originald)
    } else if (value === 3) {
      let nextd = originald.split(" ")
      // console.log(nextd)
      nextd[1] = nextd[1] - 3
      nextd[4] = nextd[4] - 3
      // console.log(bit, nextd)
      nextd = nextd.join(" ")
      // console.log(nextd)
      $(
        `g[data-eid=${ID}][data-datatype=${datatype}][data-switch=2][data-bit=${bit}] path`
      ).attr("d", nextd)
    } else if (value === 0) {
      $(
        `g[data-eid=${ID}][data-datatype=${datatype}][data-switch=2][data-bit=${bit}] path`
      ).attr("d", "")
    }
  }
}
//----------------------------------------------------------------------------
function svgFuseSet(
  ID,
  datatype,
  value,
  fuseSet,
  bit = undefined,
  bit_pos = undefined
) {
  // console.log(datatype, bit, value)
  if (value === undefined) return

  //--初始化時須計算中心參考點、圖形offset座標
  let element_type = "fuse1"
  let key = ID + "-" + datatype + "_" + element_type
  if (dict_element_referP[key] === undefined) {
    cal_reference(ID, datatype, element_type)
  }
  if (dict_element_offset[key] === undefined) {
    cal_offset(ID, datatype, element_type)
  }

  //--填入選擇器條件
  let element_select_condition = `g[data-eid=${ID}][data-datatype=${datatype}][data-fuse=1]`
  if (bit !== undefined) {
    element_select_condition += `[data-bit=${bit}]`
  }
  if (bit_pos !== undefined) {
    element_select_condition += `[data-bit-pos=${bit_pos}]`
  }

  if (fuseSet === "1") {
    // 針對每個element，暫存初始path d 變數
    for (let i = 0; i < $(element_select_condition).length; i++) {
      if (
        $(element_select_condition).eq(i).children().attr("originald") !==
        undefined
      ) {
        break
      } else {
        let d = $(element_select_condition).eq(i).children().attr("d")
        $(element_select_condition).eq(i).children().attr("originald", d)
      }
    }

    if (value === 1) {
      //保險絲健在
      for (let i = 0; i < $(element_select_condition).length; i++) {
        let origin = $(element_select_condition)
          .eq(0)
          .children()
          .attr("originald")
        let element_id = $(element_select_condition).eq(i).attr("id")
        let temp = origin.split(" ")
        let reference = dict_element_referP[key][element_id]
        let offset = dict_element_offset[key][value]
        let nextd = []
        let char_cnt = 0
        for (let j = 0; j < temp.length; j++) {
          if (isNaN(parseFloat(temp[j]))) {
            //該點為'M'或'L'或'C'
            nextd.push(temp[j])
            char_cnt++
          } else {
            switch ((j - char_cnt) % 2) {
              case 0: //x點位
                nextd.push((offset[j - char_cnt] + reference[0]).toString())
                // console.log(offset[j-char_cnt], "+", reference[0],"=", nextd[nextd.length-1]);
                break
              case 1: //y點位
                nextd.push((offset[j - char_cnt] + reference[1]).toString())
                // console.log(offset[j-char_cnt], "+", reference[1],"=", nextd[nextd.length-1]);
                break
              default:
                break
            }
          }
        }
        nextd = nextd.join(" ")
        // console.log("nextd", nextd);
        $("#" + element_id)
          .children()
          .attr("d", nextd)
      }
    } else if (value === 0) {
      //保險絲熔斷
      //console.log(element_select_condition);
      for (let i = 0; i < $(element_select_condition).length; i++) {
        let element_id = $(element_select_condition).eq(i).attr("id")
        $("#" + element_id)
          .children()
          .attr("d", "")
      }
    }
  }
}
