/**
 * VoltWay 电途
 *
 * AI智能预警API
 *
 * warning.js
 *
 */





/* ===============================
   查询AI预警
================================ */


function qryWarningList(
    level="全部"
){



    let list =
    [...aiWarnings];





    if(
        level
        &&
        level!=="全部"
    ){


        list =
        list.filter(

            item=>

            item.level===level

        );


    }

    return {
        success:true,
        count:list.length,
        data:list
    };
}

/* ===============================
   高风险设备
================================ */
function qryHighRiskPiles(){
    return aiWarnings.filter(
        item=>
        item.level==="高"
        ||
        item.level==="严重"

    );
}
/* ===============================
   建议断电设备
================================ */

function qryPowerOffRecommend(){
    return aiWarnings.filter(
        item=>
        item.type
        ===
        "异常断电"

    );
}

/* ===============================
   AI预警处理
================================ */
function handleWarning( warningId ){

    let warning =
    aiWarnings.find(
        item=> item.id===warningId
    );

    if(!warning){
        return {
            success:false,
            message:
            "预警不存在"

        };
    }

    warning.handled=true;

    warning.handleTime =
    new Date()
    .toLocaleString();

    addLog(
        "处理AI预警",
        warning.pileId
    );

    return {
        success:true,
        message:
        "处理完成"
    };
}

/* ===============================
   AI统计
================================ */
function getWarningSummary(){

    return {
        total:
        aiWarnings.length,

        serious:
        aiWarnings.filter(
            w=> w.level==="严重"
        ).length,

        high:
        aiWarnings.filter(
            w=> w.level==="高"
        ).length,

        normal:
        aiWarnings.filter(
            w=> w.level==="中"
        ).length
    };
}