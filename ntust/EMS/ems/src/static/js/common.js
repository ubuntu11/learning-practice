var $SCRIPT_ROOT = ""
//----------------------------------------------------------------------------
function setValue(obj, value, set) {
  // console.log(obj, value, set);
  if (value === undefined) {
    $(obj)
      .text(set.error !== undefined ? set.error : "---")
      .css({ color: "black" }) //error
  } else if (set.tagType === "number") {
    if (value != null) $(obj).text(round(value, set.round))
    //num4math
    else $(obj).text("NA") //num4math
  } else if (set.tagType === "text") {
    $(obj)
      .text(translate(value, set.translate, set.error))
      .css({ color: translateColor(value, set.color) }) //num2string
  } else if (set.tagType === "bits") {
    $(obj).text(value) //nothing
    set.bits.forEach((bitSet) => {
      $(`${obj}[data-bit=${bitSet.index}]`)
        .text(bitTranslate(value, bitSet, set.error))
        .css({ color: bitTranslateColor(value, bitSet) })
    })
  } else if (set.tagType === "hex") {
    $(obj).text(decimalToHexString(value)) //num2hex
  } else if (set.tagType === "hexArray" || set.tagType === "hexArray2") {
    value.forEach((i, index) => {
      $(`${obj}[data-index=${index}]`).text(decimalToHexString(i)) //num2hex
    })
  } else if (
    set.tagType === "numberArray" ||
    set.tagType === "numberArray2" ||
    set.tagType === "numberArray3"
  ) {
    value.forEach((i, index) => {
      $(`${obj}[data-index=${index}]`).text(round(i, set.round)) //num4math
    })
  } else if (set.tagType === "textArray") {
    value.forEach((i, index) => {
      $(`${obj}[data-index=${index}]`)
        .text(translate(i, set.translate, set.error))
        .css({ color: translateColor(i, set.color) }) //num2string
    })
  } else if (set.tagType === "bitsArray" || set.tagType === "bitsArray2") {
    value.forEach((i, index) => {
      $(`${obj}[data-index=${index}]`).text(i) //nothing
      set.bits.forEach((bitSet) => {
        $(`${obj}[data-index=${index}][data-bit=${bitSet.index}]`)
          .text(bitTranslate(i, bitSet, set.error))
          .css({ color: bitTranslateColor(i, bitSet) })
      })
    })
  } else if (set.tagType === "bitsList") {
    $(obj).text(bitListTranslate(value, set, set.error))
    // .css({ color: translateColor(value, set.color) });
  } else if (
    set.tagType === "bitsListArray" ||
    set.tagType === "bitsListArray2"
  ) {
    value.forEach((i, index) => {
      $(`${obj}[data-index=${index}]`).text(bitListTranslate(i, set, set.error))
      // $(`${obj}[data-index=${index}]`).text(bitListTranslate(i, set, set.error));
    })
    // .css({ color: translateColor(value, set.color) });
  } else if (set.tagType === "string") {
    $(obj).text(value) //string
  }
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
function bitListTranslate(value, set, error) {
  let result = ""
  let bitCounter = 0
  set.bits.forEach((bitContent) => {
    if (
      bitContent.translate[0] !== undefined &&
      bitContent.translate[1] !== undefined
    ) {
      if (
        bitContent.translate[0] === "False" &&
        bitContent.translate[1] === "True"
      ) {
        bitContent.translate[0] = ""
        bitContent.translate[1] = bitContent.name
      } else if (
        bitContent.translate[1] === "False" &&
        bitContent.translate[0] === "True"
      ) {
        bitContent.translate[0] = bitContent.name
        bitContent.translate[1] = ""
      }
    }
    let bitval = (value >> bitContent.index) & 1
    if (bitval != 0) {
      bitCounter += 1
      if (bitCounter > 1) {
        result += " / "
      }
    }
    result += translate(bitval, bitContent.translate, error)
  })
  return result
}
//----------------------------------------------------------------------------
function decimalToHexString(number) {
  if (number < 0) {
    number = 0xffffffff + number + 1
  }
  return "0x" + number.toString(16).toUpperCase().padStart(4, "0")
}
//----------------------------------------------------------------------------
//取得保護狀態
let protection_get = function (e) {
  $.getJSON($SCRIPT_ROOT + "/protection_get", {}, function (data) {
    const protection_data = data.data
    // console.log(protection_data)
    if (protection_data.protection === 1) {
      $("#protection").removeClass("blink").css({ "background-color": "red" })
    } else if (protection_data.protection === 2) {
      $("#protection").addClass("blink").css({ "background-color": "red" })
    } else {
      $("#protection").removeClass("blink").css({ "background-color": "green" })
    }
  })
  return false
}
//----------------------------------------------------------------------------
let alarm_get_show1_level1_checktimeNull = function (e) {
  //取得 即時 警告 未確認 告警數 至多10筆
  $.getJSON(
    $SCRIPT_ROOT + "/alarm_get",
    {
      limit: 10,
      start: 0,
      page: 0,
      condition: JSON.stringify({
        show: {
          1: true,
          2: false,
        },
        level: {
          1: true,
          2: false,
        },
        checktime: null,
      }),
      datepicker1: "",
      datepicker2: "",
      search: 0,
    },
    function (data) {
      let alarm_data = data.data
      let count = data.total
      $("#warningBox").empty()
      alarm_data.forEach((alarm, index) => {
        $("#warningBox").append(
          $("<a/>")
            .addClass("dropdown-item")
            .attr("href", "/alarm")
            .append(
              $("<div/>")
                .addClass("d-flex")
                .html(
                  `<h6 class="font-weight-bold">項次:&nbsp;</h6><h6>${
                    index + 1
                  }</h6>`
                ),
              $("<div/>")
                .addClass("d-flex")
                .html(
                  `<h6 class="font-weight-bold">時間:&nbsp;</h6><h6>${alarm.time}</h6>`
                ),
              $("<div/>")
                .addClass("d-flex")
                .html(
                  `<h6 class="font-weight-bold">地點:&nbsp;</h6><h6>${alarm.place}</h6>`
                ),
              $("<div/>")
                .addClass("d-flex")
                .html(
                  `<h6 class="font-weight-bold">設備名稱:&nbsp;</h6><h6>${alarm.equip}</h6>`
                ),
              $("<div/>")
                .addClass("d-flex")
                .html(
                  `<h6 class="font-weight-bold">時間:&nbsp;</h6><h6>${alarm.event}</h6>`
                ),
              $("<hr/>").addClass("solid")
            )
        )
      })
      $("#warningBox").append(
        $("<a/>").addClass("dropdown-item").attr("href", "/alarm").html("更多")
      )
      $("#warningCount").text(count)
    }
  )
  return false
}
//----------------------------------------------------------------------------
let alarm_get_show1_level2_checktimeNull = function (e) {
  //取得 即時 故障 未確認 告警數 至多10筆
  $.getJSON(
    $SCRIPT_ROOT + "/alarm_get",
    {
      limit: 10,
      start: 0,
      page: 0,
      condition: JSON.stringify({
        show: {
          1: true,
          2: false,
        },
        level: {
          1: false,
          2: true,
        },
        checktime: null,
      }),
      datepicker1: "",
      datepicker2: "",
      search: 0,
    },
    function (data) {
      let alarm_data = data.data
      let count = data.total
      $("#faultBox").empty()
      alarm_data.forEach((alarm, index) => {
        $("#faultBox").append(
          $("<a/>")
            .addClass("dropdown-item")
            .attr("href", "/alarm")
            .append(
              $("<div/>")
                .addClass("d-flex")
                .html(
                  `<h6 class="font-weight-bold">項次:&nbsp;</h6><h6>${
                    index + 1
                  }</h6>`
                ),
              $("<div/>")
                .addClass("d-flex")
                .html(
                  `<h6 class="font-weight-bold">時間:&nbsp;</h6><h6>${alarm.time}</h6>`
                ),
              $("<div/>")
                .addClass("d-flex")
                .html(
                  `<h6 class="font-weight-bold">地點:&nbsp;</h6><h6>${alarm.place}</h6>`
                ),
              $("<div/>")
                .addClass("d-flex")
                .html(
                  `<h6 class="font-weight-bold">設備名稱:&nbsp;</h6><h6>${alarm.equip}</h6>`
                ),
              $("<div/>")
                .addClass("d-flex")
                .html(
                  `<h6 class="font-weight-bold">時間:&nbsp;</h6><h6>${alarm.event}</h6>`
                ),
              $("<hr/>").addClass("solid")
            )
        )
      })
      $("#faultBox").append(
        $("<a/>")
          .addClass("dropdown-item")
          .attr("href", "/alarm")
          .css({ color: "blue" })
          .html("更多")
      )
      $("#faultCount").text(count)
    }
  )
  return false
}
//--------------------------------------------------------------------------------------------------------------
let ShowTime = function (e) {
  //
  $.getJSON($SCRIPT_ROOT + "/ShowTime", {}, function (data) {
    let date = data.now.split(" ")[0].split("-")
    $(display_date).text(`${date[1]}/${date[2]}`)
    $(display_time).text(data.now.split(" ")[1])
    setTimeout("ShowTime()", 1000)
  })
  return false
}
//--------------------------------------------------------------------------------------------------------------
function checkTime(i) {
  if (i < 10) {
    i = "0" + i
  }
  return i
}
//-----------------------------------------------------------------------
function pad(number) {
  if (number < 10) {
    return "0" + number
  }
  return number
}
//-----------------------------------------------------------------------
function c_chooseday(id) {
  var id = "#" + id
  $(id).datepicker({
    showOtherMonths: true,
    selectOtherMonths: true,
    changeYear: true,
    changeMonth: true,
    dateFormat: "yy-mm-dd",
    maxDate: "new Date()",
  })

  var dt = new Date()
  var Month = [
    "01",
    "02",
    "03",
    "04",
    "05",
    "06",
    "07",
    "08",
    "09",
    "10",
    "11",
    "12",
  ]
  var date =
    dt.getFullYear() +
    "-" +
    Month[dt.getMonth()] +
    "-" +
    checkTime(dt.getDate())
  $(id).val(date)
}
//-----------------------------------------------------------------------
function c_choosehour(id) {
  var id = "#" + id
  $(id).datetimepicker({
    showOtherMonths: true,
    selectOtherMonths: true,
    changeYear: true,
    changeMonth: true,
    format: "Y-m-d H:i",
    maxDate: "new Date()",
  })
  var dt = new Date()
  var Month = [
    "01",
    "02",
    "03",
    "04",
    "05",
    "06",
    "07",
    "08",
    "09",
    "10",
    "11",
    "12",
  ]
  var date =
    dt.getFullYear() +
    "-" +
    Month[dt.getMonth()] +
    "-" +
    checkTime(dt.getDate()) +
    " 00:00"
  $(id).val(date)
}
//-----------------------------------------------------------------------
function c_choosehour2(id, dt, minusDate = "-1970/1/1") {
  const _id = "#" + id
  $(_id).datetimepicker({
    showOtherMonths: true,
    selectOtherMonths: true,
    changeYear: true,
    changeMonth: true,
    format: "Y-m-d H:i",
    minDate: minusDate,
  })
  const Month = [
    "01",
    "02",
    "03",
    "04",
    "05",
    "06",
    "07",
    "08",
    "09",
    "10",
    "11",
    "12",
  ]
  if (dt === undefined) {
    dt = new Date()
  }
  let date =
    dt.getFullYear() +
    "-" +
    Month[dt.getMonth()] +
    "-" +
    checkTime(dt.getDate()) +
    " 00:00"
  $(_id).val(date)
}
//-------------------------------------------------------------------------------------------------------------------------------
function DateAdd(interval, number, date) {
  switch (interval) {
    case "y ": {
      date.setFullYear(date.getFullYear() + number)
      return date
      break
    }
    case "q ": {
      date.setMonth(date.getMonth() + number * 3)
      return date
      break
    }
    case "m ": {
      date.setMonth(date.getMonth() + number)
      return date
      break
    }
    case "w ": {
      date.setDate(date.getDate() + number * 7)
      return date
      break
    }
    case "d ": {
      date.setDate(date.getDate() + number)
      return date
      break
    }
    case "h ": {
      date.setHours(date.getHours() + number)
      return date
      break
    }
    case "m ": {
      date.setMinutes(date.getMinutes() + number)
      return date
      break
    }
    case "s ": {
      date.setSeconds(date.getSeconds() + number)
      return date
      break
    }
    default: {
      date.setDate(d.getDate() + number)
      return date
      break
    }
  }
}
//------------------------------------------------------------------------------------------------------------------------------
function calendarsetdate() {
  var date = new Date()
  var Month = [
    "01",
    "02",
    "03",
    "04",
    "05",
    "06",
    "07",
    "08",
    "09",
    "10",
    "11",
    "12",
  ]
  if ($($(this).data("value")).val().includes(":")) var time = " 00:00"
  else var time = ""
  date =
    date.getFullYear() +
    "-" +
    Month[date.getMonth()] +
    "-" +
    checkTime(date.getDate()) +
    time
  $($(this).data("value")).val(date)
}
//-------------------------------------------------------------------------------------------------------------------------------
function calendardateleft2() {
  if (!$($(this).data("value")).val().includes(":"))
    var date = ($($(this).data("value")).val() + " 00:00:00").replace(/-/g, "/")
  else var date = $($(this).data("value")).val().replace(/-/g, "/")
  date = DateAdd("m ", -1, new Date(date))
  var Month = [
    "01",
    "02",
    "03",
    "04",
    "05",
    "06",
    "07",
    "08",
    "09",
    "10",
    "11",
    "12",
  ]
  if ($($(this).data("value")).val().includes(":"))
    var time = " " + pad(date.getHours()) + ":" + pad(date.getMinutes())
  else var time = ""
  date =
    date.getFullYear() +
    "-" +
    Month[date.getMonth()] +
    "-" +
    checkTime(date.getDate()) +
    time
  $($(this).data("value")).val(date)
}
//-------------------------------------------------------------------------------------------------------------------------------
function calendardateleft() {
  if (!$($(this).data("value")).val().includes(":"))
    var date = ($($(this).data("value")).val() + " 00:00:00").replace(/-/g, "/")
  else var date = $($(this).data("value")).val().replace(/-/g, "/")
  date = DateAdd("d ", -1, new Date(date))
  var Month = [
    "01",
    "02",
    "03",
    "04",
    "05",
    "06",
    "07",
    "08",
    "09",
    "10",
    "11",
    "12",
  ]
  if ($($(this).data("value")).val().includes(":"))
    var time = " " + pad(date.getHours()) + ":" + pad(date.getMinutes())
  else var time = ""
  date =
    date.getFullYear() +
    "-" +
    Month[date.getMonth()] +
    "-" +
    checkTime(date.getDate()) +
    time
  $($(this).data("value")).val(date)
}
//-------------------------------------------------------------------------------------------------------------------------------
function calendarhourleft() {
  if (!$($(this).data("value")).val().includes(":"))
    var date = ($($(this).data("value")).val() + " 00:00:00").replace(/-/g, "/")
  else var date = $($(this).data("value")).val().replace(/-/g, "/")
  date = DateAdd("h ", -1, new Date(date))
  var Month = [
    "01",
    "02",
    "03",
    "04",
    "05",
    "06",
    "07",
    "08",
    "09",
    "10",
    "11",
    "12",
  ]
  if ($($(this).data("value")).val().includes(":"))
    var time = " " + pad(date.getHours()) + ":" + pad(date.getMinutes())
  else var time = ""
  date =
    date.getFullYear() +
    "-" +
    Month[date.getMonth()] +
    "-" +
    checkTime(date.getDate()) +
    time
  $($(this).data("value")).val(date)
}
//-------------------------------------------------------------------------------------------------------------------------------
function calendardateright2() {
  if (!$($(this).data("value")).val().includes(":"))
    var date = ($($(this).data("value")).val() + " 00:00:00").replace(/-/g, "/")
  else var date = $($(this).data("value")).val().replace(/-/g, "/")
  date = DateAdd("m ", 1, new Date(date))
  var dt = new Date()
  dt.setMinutes(0)
  var Month = [
    "01",
    "02",
    "03",
    "04",
    "05",
    "06",
    "07",
    "08",
    "09",
    "10",
    "11",
    "12",
  ]
  var localdate =
    dt.getFullYear() +
    "/" +
    Month[dt.getMonth()] +
    "/" +
    checkTime(dt.getDate()) +
    " " +
    pad(dt.getHours()) +
    ":00:00"
  if (Date.parse(date).valueOf() > Date.parse(localdate).valueOf()) {
    date = dt
  }
  if ($($(this).data("value")).val().includes(":"))
    var time = " " + pad(date.getHours()) + ":" + pad(date.getMinutes())
  else var time = ""
  date =
    date.getFullYear() +
    "-" +
    Month[date.getMonth()] +
    "-" +
    checkTime(date.getDate()) +
    time
  $($(this).data("value")).val(date)
}
//-------------------------------------------------------------------------------------------------------------------------------
function calendardateright() {
  if (!$($(this).data("value")).val().includes(":"))
    var date = ($($(this).data("value")).val() + " 00:00:00").replace(/-/g, "/")
  else var date = $($(this).data("value")).val().replace(/-/g, "/")
  date = DateAdd("d ", 1, new Date(date))
  var dt = new Date()
  dt.setMinutes(0)
  var Month = [
    "01",
    "02",
    "03",
    "04",
    "05",
    "06",
    "07",
    "08",
    "09",
    "10",
    "11",
    "12",
  ]
  var localdate =
    dt.getFullYear() +
    "/" +
    Month[dt.getMonth()] +
    "/" +
    checkTime(dt.getDate()) +
    " " +
    pad(dt.getHours()) +
    ":00:00"
  if (Date.parse(date).valueOf() > Date.parse(localdate).valueOf()) {
    date = dt
  }
  if ($($(this).data("value")).val().includes(":"))
    var time = " " + pad(date.getHours()) + ":" + pad(date.getMinutes())
  else var time = ""
  date =
    date.getFullYear() +
    "-" +
    Month[date.getMonth()] +
    "-" +
    checkTime(date.getDate()) +
    time
  $($(this).data("value")).val(date)
}
//-------------------------------------------------------------------------------------------------------------------------------
function calendarhourright() {
  if (!$($(this).data("value")).val().includes(":"))
    var date = ($($(this).data("value")).val() + " 00:00:00").replace(/-/g, "/")
  else var date = $($(this).data("value")).val().replace(/-/g, "/")
  date = DateAdd("h ", 1, new Date(date))
  var dt = new Date()
  dt.setMinutes(0)
  var Month = [
    "01",
    "02",
    "03",
    "04",
    "05",
    "06",
    "07",
    "08",
    "09",
    "10",
    "11",
    "12",
  ]
  var localdate =
    dt.getFullYear() +
    "/" +
    Month[dt.getMonth()] +
    "/" +
    checkTime(dt.getDate()) +
    " " +
    pad(dt.getHours()) +
    ":00:00"
  if (Date.parse(date).valueOf() > Date.parse(localdate).valueOf()) {
    date = dt
  }
  if ($($(this).data("value")).val().includes(":"))
    var time = " " + pad(date.getHours()) + ":" + pad(date.getMinutes())
  else var time = ""
  date =
    date.getFullYear() +
    "-" +
    Month[date.getMonth()] +
    "-" +
    checkTime(date.getDate()) +
    time
  $($(this).data("value")).val(date)
}
//------------------------------------------------------------------------------------------------------------------------
function calendarsetdate2() {
  var date = new Date()
  var Month = [
    "01",
    "02",
    "03",
    "04",
    "05",
    "06",
    "07",
    "08",
    "09",
    "10",
    "11",
    "12",
  ]
  date =
    date.getFullYear() +
    "-" +
    Month[date.getMonth()] +
    "-" +
    checkTime(date.getDate())
  $($(this).data("value")).val(date)
  $($(this).data("value")).trigger("change")
}
//----------------------------------------------------------------------------------------------------------------------------
function GetCheckedValue(checkboxclass) {
  var _id = "." + checkboxclass + ":checked"
  return $(_id)
    .map(function () {
      return $(this).val()
    })
    .get()
}
//-----------------------------------------------------------------------------------------------------------------
let history_data_list = function (e) {
  $.getJSON(
    $SCRIPT_ROOT + "/history_data_list",
    {
      history_ID: history_ID,
      datacollection: datacollection,
      datatype: datatype,
      datepicker1: $("#datepicker1").val() + " 00:00:00",
      datepicker2: $("#datepicker2").val() + " 00:00:00",
      date_range: $("#date_range").val(),
      interval: interval,
      tagType: tagType,
      plotType: plotType,
      module_rack_select: $(".module.Rack_select").prop("selectedIndex"),
      cell_rack_select: $(".cell.Rack_select").prop("selectedIndex"),
      cell_module_select: $(".cell.Module_select").prop("selectedIndex"),
    },
    function (data) {
      // alert("S")
      console.log(data)
      $("#history_loading").addClass("d-none").removeClass("d-flex")
      $("#history_plot")
      if (
        tagType === "numberArray" ||
        tagType === "numberArray2" ||
        tagType === "numberArray3"
      )
        plot_multi(
          "history_plot",
          data.x_axis,
          data.y_axis,
          history_name,
          data.name,
          plotType,
          y_title
        )
      else
        plot(
          "history_plot",
          data.x_axis,
          data.y_axis,
          history_name,
          datatype,
          plotType,
          y_title
        )
    }
  )
  return false
}
//-----------------------------------------------------------------------------------------------------------------
function history_info(y_axis) {}
// Capture the event emission
//-----------------------------------------------------------------------------------------------------------------
function oneline_download() {
  var dt = new Date()
  var Month = [
    "01",
    "02",
    "03",
    "04",
    "05",
    "06",
    "07",
    "08",
    "09",
    "10",
    "11",
    "12",
  ]
  var h = dt.getHours()
  var m = dt.getMinutes()
  var s = dt.getSeconds()
  m = checkTime(m)
  s = checkTime(s)
  var date =
    dt.getFullYear() +
    "-" +
    Month[dt.getMonth()] +
    "-" +
    checkTime(dt.getDate()) +
    " " +
    h +
    ":" +
    m +
    ":" +
    s

  var csvList = []
  var memberContent = ""
  var csvContent

  this.download = "export" + ".csv"

  var gd = document.getElementById("history_plot")

  console.log(gd.data)
  var regList = ["number", "time"]

  var x = gd.data[0].x
  for (var i = 0; i < gd.data.length; i++) {
    if (gd.data[i].y.length === x.length) regList.push(gd.data[i].name)
  }
  csvList.push(regList)

  for (j = 0; j < x.length; j++) {
    var regList = []
    regList.push(j + 1)
    regList.push(x[j])
    for (var i = 0; i < gd.data.length; i++) {
      if (gd.data[i].y.length === x.length) regList.push(gd.data[i].y[j])
    }
    csvList.push(regList)
  }

  console.log(csvList)
  // 產生 csv 內容
  csvList.forEach(function (rowArray) {
    var row = rowArray.join(",")
    memberContent += row + "\r\n"
  })

  // 產生 csv Blob
  csvContent = URL.createObjectURL(
    new Blob(["\uFEFF" + memberContent], {
      type: "text/csv;charset=utf-8;",
    })
  )

  // 產生 csv 連結
  this.href = csvContent
}
//------------------------------------------------------------
function apparentPowerLimit(data, data2, capacity, info) {
  var data = parseFloat(data)
  // console.log(data, min, max, isNaN(data));
  if (isNaN(data)) {
    return "請輸入數值"
  } else if (data == NaN || data == undefined) {
    return "請輸入數值"
  } else if (isNaN(data2) && data > capacity) {
    return info + ",超過最大容量" + capacity
  } else if (Math.sqrt(Math.pow(data, 2) + Math.pow(data2, 2)) > capacity) {
    return info + ",視功率超過最大容量" + capacity
  } else {
    return false
  }
}
//------------------------------------------------------------
function limit(data, min, max, info) {
  var data = parseFloat(data)
  // console.log(data, min, max, isNaN(data));
  if (isNaN(data)) {
    return "請輸入數值"
  } else if (data == NaN || data == undefined) {
    return "請輸入數值"
  } else if (data > max) {
    return info + ",超過最大值" + max
  } else if (data < min) {
    return info + ",小於最小值" + min
  } else {
    return false
  }
}
//--------------------------------------------------------
function status2string(status) {
  switch (status) {
    case 0:
      return "正常"
    case 1:
      return "異常"
    default:
      return "---"
  }
}
//--------------------------------------------------------
function status2color(status) {
  switch (status) {
    case 0:
      return ""
    case 1:
      return "rgba(255,0,0,1)"
    default:
      return "#ff0000b7"
  }
}
//--------------------------------------------------------
function count2time(count) {
  t = count
  h = parseInt(t / 3600)
  m = parseInt((t - h * 3600) / 60)
  s = parseInt(t - h * 3600 - m * 60)
  return checkTime(h) + ":" + checkTime(m) + ":" + checkTime(s)
}
//----------------------------------------------------------------------------------------------------
function plot(id, x, y, title, name, type, y_title, pf = false) {
  //console.log(pf)
  if (pf == false) {
    var trace1 = {
      type: type,
      mode: "lines",
      name: name,
      x: x,
      y: y,
      line: { color: "#17BECF" },
    }
    var yaxis_set = {
      title: {
        text: y_title,
      },
      rangemode: "tozero",
      autorange: true,
      type: "linear",
    }
  } else {
    var text = []
    for (var i in y) {
      text.push(y[i])
    }
    y = pfvalue(y)

    var trace1 = {
      type: type,
      mode: "lines",
      name: name,
      hoverinfo: "text",
      x: x,
      y: y,
      text: text,
      line: { color: "#17BECF" },
    }
    var yaxis_set = {
      title: {
        text: y_title,
      },
      autorange: true,
      type: "linear",
      tickmode: "array", // If "array", the placement of the ticks is set via `tickvals` and the tick text is `ticktext`.
      tickvals: [-1, -0.75, -0.5, -0.25, 0, 0.25, 0.5, 0.75, 1],
      ticktext: [
        "0(lag)",
        "-0.25",
        "-0.5",
        "-0.75",
        "1",
        "0.75",
        "0.5",
        "0.25",
        "0(lead)",
      ],
    }
  }

  var data = [trace1]
  var layout = {
    title: {
      text: title,
      font: {
        family: "Courier New, monospace",
        size: 30,
      },
    },
    xaxis: {},
    yaxis: yaxis_set,
  }
  Plotly.newPlot(id, data, layout, {
    displaylogo: false,
    modeBarButtonsToRemove: [
      "sendDataToCloud",
      "hoverClosestCartesian",
      "hoverCompareCartesian",
      "toggleSpikelines",
      "autoScale2d",
    ],
  })
}
function plot_multi_yaxis(
  id,
  x,
  y,
  title,
  name,
  y_choice,
  type = "scatter",
  y_title,
  pf = false
) {
  var data = []
  var yaxis_set1 = {}
  var yaxis_set2 = {}
  //---------------------------------------------------
  //補0，以方便判斷哪格checkbox被勾選
  if (y_choice == 0) {
    var BinaryStr = "000000"
    yaxis_set1 = {
      title: {
        text: y_title,
      },
      type: "linear",
    }
  } else {
    var BinaryStr = parseInt(y_choice).toString(2)
    while (BinaryStr.length < 6) {
      BinaryStr = "0" + BinaryStr
    }
  }
  //---------------------------------------------------
  if (pf == false) {
    for (var i in y) {
      //---------------------------------------------
      //開始判斷哪個checkbox被勾選
      if (BinaryStr[BinaryStr.length - i - 1] == 1) {
        data.push({
          type: type,
          mode: "lines",
          name: name[i],
          x: x,
          y: y[i],
          yaxis: "y2",
        })
        //---------------------------------------------
      } else {
        //---------------------------------------------
        //哪個checkbox沒被勾選
        data.push({
          type: type,
          mode: "lines",
          name: name[i],
          x: x,
          y: y[i],
        })
        //---------------------------------------------
      }
    }
  } else {
    //console.log(y)
    for (var i in y) {
      var text = []

      for (var j in y[i]) {
        text.push(y[i][j])
      }
      //console.log(text)
      var _y = pfvalue(y[i])
      //console.log(_y)
      data.push({
        type: type,
        mode: "lines",
        name: name[i],
        hoverinfo: "text",
        text: text,
        x: x,
        y: _y,
        //line: {color: color[i]}
      })
    }
    var yaxis_set = {
      title: {
        text: y_title,
      },

      autorange: true,
      type: "linear",
      tickmode: "array", // If "array", the placement of the ticks is set via `tickvals` and the tick text is `ticktext`.
      tickvals: [-1, -0.75, -0.5, -0.25, 0, 0.25, 0.5, 0.75, 1],
      ticktext: [
        "0(lag)",
        "-0.25",
        "-0.5",
        "-0.75",
        "1",
        "0.75",
        "0.5",
        "0.25",
        "0(lead)",
      ],
    }
  }
  yaxis_set1 = {
    title: {
      text: y_title,
    },
    type: "linear",
    autorange: true,
  }
  yaxis_set2 = {
    title: {
      text: y_title,
    },
    type: "linear",
    autorange: true,
    side: "right",
    overlaying: "y",
  }
  var layout = {
    title: {
      text: title,
      font: {
        family: "Courier New, monospace",
        size: 30,
      },
    },
    xaxis: {},
    yaxis: yaxis_set1,
    yaxis2: yaxis_set2,
  }
  Plotly.newPlot(id, data, layout, {
    displaylogo: false,
    modeBarButtonsToRemove: [
      "sendDataToCloud",
      "hoverClosestCartesian",
      "hoverCompareCartesian",
      "toggleSpikelines",
      "autoScale2d",
    ],
  })
}
//---------------------------------------------------------------------------------------------------
function plot_multi(
  id,
  x,
  y,
  title,
  name,
  type = "scatter",
  y_title,
  pf = false
) {
  var data = []
  if (pf == false) {
    for (var i in y) {
      data.push({
        type: type,
        mode: "lines",
        name: name[i],
        x: x,
        y: y[i],
        //line: {color: color[i]}
      })
    }
    var yaxis_set = {
      title: {
        text: y_title,
      },
      rangemode: "tozero",
      autorange: true,
      type: "linear",
    }
  } else {
    //console.log(y)
    for (var i in y) {
      var text = []

      for (var j in y[i]) {
        text.push(y[i][j])
      }
      //console.log(text)
      var _y = pfvalue(y[i])
      //console.log(_y)
      data.push({
        type: type,
        mode: "lines",
        name: name[i],
        hoverinfo: "text",
        text: text,
        x: x,
        y: _y,
        //line: {color: color[i]}
      })
    }
    var yaxis_set = {
      title: {
        text: y_title,
      },

      autorange: true,
      type: "linear",
      tickmode: "array", // If "array", the placement of the ticks is set via `tickvals` and the tick text is `ticktext`.
      tickvals: [-1, -0.75, -0.5, -0.25, 0, 0.25, 0.5, 0.75, 1],
      ticktext: [
        "0(lag)",
        "-0.25",
        "-0.5",
        "-0.75",
        "1",
        "0.75",
        "0.5",
        "0.25",
        "0(lead)",
      ],
    }
  }

  var layout = {
    title: {
      text: title,
      font: {
        family: "Courier New, monospace",
        size: 30,
      },
    },
    xaxis: {},
    yaxis: yaxis_set,
  }
  Plotly.newPlot(id, data, layout, {
    displaylogo: false,
    modeBarButtonsToRemove: [
      "sendDataToCloud",
      "hoverClosestCartesian",
      "hoverCompareCartesian",
      "toggleSpikelines",
      "autoScale2d",
    ],
  })
}
//-----------------------------------------------------------------------------------------
function plot_twoline_onepoint(
  id,
  x,
  y,
  x_range,
  text,
  title,
  x_title,
  y_title
) {
  var data = []
  var color = ["#FF0000", "#0066FF", "Green"]

  for (var i = 0; i < 2; i++) {
    data.push({
      type: "scatter",
      mode: "lines+text",
      x: x[i],
      y: y[i],
      hoverinfo: "x+y",
      text: text[i],
      line: { color: color[i] },
    })
  }
  data.push({
    type: "scatter",
    mode: "markers",
    marker: { size: 8 },
    x: x[2],
    y: y[2],
    hoverinfo: "x+y",
    line: { color: color[i] },
  })
  var layout = {
    title: {
      text: title,
      font: {
        family: "Courier New, monospace",
        size: 24,
      },
    },
    showlegend: false,
    xaxis: {
      title: {
        text: x_title,
      },
      //autorange: true,
      range: x_range,
    },
    yaxis: {
      title: {
        text: y_title,
      },
      range: [-110, 110],
      type: "linear",
    },
    hovermode: "closest",
  }
  Plotly.newPlot(id, data, layout, {
    displaylogo: false,
    modeBarButtonsToRemove: [
      "sendDataToCloud",
      "hoverClosestCartesian",
      "hoverCompareCartesian",
      "toggleSpikelines",
    ],
  })
}
//-----------------------------------------------------------------------------------------
function createArrayTr(obj, length) {
  $(`${obj}`)
    .toArray()
    .forEach((i) => {
      const preText = $(i).attr("data-preText")
      const nextText = $(i).attr("data-nextText")
      for (let j = length; j > 1; j--) {
        // console.log($(`${i[0]} th`).clone())
        // console.log($(i).clone().children(".set"))
        if (preText !== undefined) {
          let td_clone = $(i).clone()
          td_clone.find(".set").attr("data-index", j - 1)
          $(i).after(
            $("<tr>")
              .addClass("clone")
              .append(
                $("<th/>").text(`${preText}${j}${nextText}`),
                td_clone.children()
                // $(i)
                //   .clone()
                //   // .children(".set").attr("data-index", j - 1)
                //   .children().find(".set").attr("data-index", j - 1).parent()
                //   // .children($(".set").attr("data-index", j - 1))
              )
          )
        }
      }
      if (preText !== undefined) {
        $(i)
          .prepend(
            $("<th/>").addClass("textIndex").text(`${preText}1${nextText}`)
          )
          .find(".set")
          .attr("data-index", 0)
        // .children($(this).find(".set").attr("data-index", 0));
        // .children($(".set").attr("data-index", 0));
      }
    })
}
//---------------------------------------------------------------------------------------------------
function plot_sys_curve(
  id,
  x,
  y,
  title,
  names,
  type = "scatter",
  y_titles,
  update = false,
  pf = false
) {
  if (update === false) {
    let data = []
    for (let i in y) {
      let xaxis_data, xaxis, yaxis
      if (y[i].length === 2) xaxis_data = [x[0], x[x.length - 1]]
      else xaxis_data = x
      // if (i == 1) xaxis = "x2"
      // else xaxis = "x"
      if (i == 1) yaxis = "y2"
      else if (i > 1) yaxis = "y3"
      else yaxis = "y"
      data.push({
        type: type,
        mode: "lines",
        name: names[i],
        x: xaxis_data,
        y: y[i],
        xaxis: "x",
        yaxis: yaxis,
        //line: {color: color[i]}
      })
    }
    console.log(data)
    var yaxis_set = {
      hovermode: "closest",
      title: {
        text: y_titles[0],
      },
      autorange: true,
      type: "linear",
      domain: [0, 0.7],
    }
    var yaxis2_set = {
      hovermode: "closest",
      title: {
        text: y_titles[1],
      },
      autorange: true,
      type: "linear",
      domain: [0.8, 1],
    }
    var yaxis3_set = {
      side: "right",
      title: {
        text: y_titles[2],
      },
      overlaying: "y",
      autorange: true,
      type: "linear",
      domain: [0, 0.7],
    }

    var layout = {
      title: {
        text: title,
        font: {
          family: "Courier New, monospace",
          size: 30,
        },
      },
      xaxis: { domain: [0, 0.9] },
      yaxis: yaxis_set,
      yaxis2: yaxis2_set,
      yaxis3: yaxis3_set,
      grid: {
        rows: 2,
        columns: 1,
        pattern: "independent",
        roworder: "bottom to top",
        subplots: [["xy"], ["xy2"], ["xy3"]],
      },
    }
    Plotly.newPlot(id, data, layout, {
      displaylogo: false,
      modeBarButtonsToRemove: [
        "sendDataToCloud",
        "hoverClosestCartesian",
        "hoverCompareCartesian",
        "toggleSpikelines",
        "autoScale2d",
      ],
    })
  } else {
    let x_axis = []
    let traces = []
    while (x_axis.length < y.length) {
      traces.push(x_axis.length)
      x_axis.push(x)
    }
    Plotly.extendTraces(id, { x: x_axis, y: y }, traces, 10800)
  }
}
function fillArray(value, len) {
  if (len == 0) return []
  var a = [value]
  while (a.length * 2 <= len) a = a.concat(a)
  if (a.length < len) a = a.concat(a.slice(0, len - a.length))
  return a
}
//---------------------------------------------------
// 更新狀態燈號
function updateStatus(
  url,
  elements,
  id_tagname,
  status_elements,
  color_tagname
) {
  let status_ID = []
  $.each(elements, function (index, element) {
    status_ID.push($(element).attr(id_tagname))
  })
  // console.log(status_ID)
  $.getJSON(
    $SCRIPT_ROOT + url,
    { status_ID: JSON.stringify(status_ID) },
    function (data) {
      // console.log(data.statuss)
      $.each(status_elements, function (index, commStatus) {
        if (data.statuss[status_ID[index]] === 1)
          $(commStatus).css(color_tagname, "rgb(179,179,179)")
        else if (data.statuss[status_ID[index]] === 2)
          $(commStatus).css(color_tagname, "#F2E200")
        else if (data.statuss[status_ID[index]] === 3)
          $(commStatus).css(color_tagname, "#FF8000")
        else $(commStatus).css(color_tagname, "rgb(0, 204, 102)")
      })
    }
  )
}
