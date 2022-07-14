 // 获取时间
 function time() {
    var timePath
    $.ajax({
        type: "GET",
        data: {
            identifyCard: '4124200009228823',
            key: 'dHJhY2tTZWFyY2gtYmFpd2FuZw=='
        },
        url: prefix + "/generateTrackCapture",
        async: false,
        error: function (request) {
            console.log(request);
        },
        success: (dataTime) => {
            if (dataTime != null) {
                // 时间路径
                timePath = dataTime
            }
        }
    })
    return timePath
}
const dataTrackTime = time();







