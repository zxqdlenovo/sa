﻿#include "FunNum.h"
#include <memory>
#include "sa_fun_core.h"
#include "sa_fun_num.h"
#include "SAAbstractDatas.h"
#include "SAPropertySetDialog.h"
#include "SAUIReflection.h"
#include "SAUIInterface.h"
#include "SAAbstractDatas.h"
#include "SAVariantDatas.h"
#include "SAValueManager.h"
#include "SAFigureWindow.h"
#include "SAChart2D.h"
#include "SAChart.h"
#include "SAVectorInterval.h"
#include "SAGUIGlobalConfig.h"
#include "ui_opt.h"
#include <QMdiSubWindow>
#include <QTextStream>
#define TR(str)\
    QApplication::translate("FunStatistics", str, 0)

FunNum::FunNum()
{

}

void FunNum::statistics()
{
    SAUIInterface::LastFocusType ft = saUI->lastFocusWidgetType();
    if(SAUIInterface::FigureWindowFocus == ft)
    {
        statisticsInChart();
    }
    else
    {
        statisticsInValue();
    }
}
///
/// \brief 求统计参数
///
void FunNum::statisticsInValue()
{
    SAAbstractDatas* data = saUI->getSelectSingleData();
    if(nullptr == data)
    {
        return;
    }
    std::shared_ptr<SATableVariant> res = saFun::statistics(data);
    if(nullptr == res)
    {
        saUI->showErrorMessageInfo(saFun::getLastErrorString());
        saUI->raiseMessageInfoDock();
        return;
    }
    res->setName(TR("%1_statistics").arg(data->getName()));
    saValueManager->addData(res);
    saUI->showNormalMessageInfo(TR("%1 sum is %2").arg(data->getName()).arg(res->getName()));

}
void FunNum::statisticsInChart()
{
    QList<QwtPlotItem*> curs;
    SAChart2D* chart = filter_xy_series(curs);
    if(nullptr == chart || curs.size() <= 0)
    {
        saUI->showMessageInfo(TR("unsupport chart items"),SA::WarningMessage);
        return;
    }

    QStringList infos;
    for (int i = 0;i<curs.size();++i)
    {
        QwtPlotItem* item = curs[i];
        QVector<double> ys;
        if(chart->isRegionVisible())
        {
            if(!chart->getXYDataInRange(nullptr,&ys,nullptr,item))
            {
                continue;
            }
        }
        else
        {
            if(!chart->getXYData(nullptr,&ys,item))
            {
                continue;
            }
        }
        if(0 == ys.size())
        {
            continue;
        }
        QMap<QString,double> res = saFun::statistics(ys);
        QString resultReport;
        QTextStream st(&resultReport,QIODevice::Text|QIODevice::ReadWrite);
        st << "<div>" << TR("statistics([\"") <<item->title().text() <<"\"])" << "</div>"
           << "<div>" << TR("sum:") << res[IDS_SUM] << "</div>"
           << "<div>" << TR("mean:")<<res[IDS_MEAN]<<"</div>"
           << "<div>" << TR("var:")<<res[IDS_VAR]<<"</div>"
           << "<div>" << TR("std:")<<res[IDS_STD]<<"</div>"
           << "<div>" << TR("skewness:")<<res[IDS_SKEWNESS]<<"</div>"
           << "<div>" << TR("kurtosis:")<<res[IDS_KURTOSIS]<<"</div>"
           << "<div>" << TR("peak to peak:")<<res[IDS_PEAK2PEAK]<<"</div>"
                            ;

        infos.append(resultReport);
    }
    if(infos.size() > 0)
    {
        saUI->showNormalMessageInfo(infos.join("================\n"));
        saUI->raiseMessageInfoDock();
    }
}



///
/// \brief 求差分
///
void FunNum::diff()
{
    SAAbstractDatas* data = saUI->getSelectSingleData();
    if(nullptr == data)
    {
        return;
    }
    SAPropertySetDialog dlg(saUI->getMainWindowPtr(),static_cast<SAPropertySetDialog::BrowserType>(SAGUIGlobalConfig::getDefaultPropertySetDialogType()));
    dlg.appendGroup(TR("property set"));
    auto tmp = dlg.appendIntProperty(TR("diff count")
                             ,1,std::numeric_limits<int>::max()
                             ,1,TR("diff count"));
    dlg.recorder("diff",tmp);
    int diffCount = dlg.getDataByID<int>("diff");
    std::shared_ptr<SAVectorDouble> res = saFun::diff(data,diffCount);
    if(nullptr == res)
    {
        saUI->showErrorMessageInfo(saFun::getLastErrorString());
        saUI->raiseMessageInfoDock();
        return;
    }
    res->setName(TR("%1_diff%2").arg(data->getName()).arg(diffCount));
    saValueManager->addData(res);
    saUI->showNormalMessageInfo(TR("%1 diff(%2) is %3")
                                .arg(data->getName())
                                .arg(diffCount)
                                .arg(res->getName()));
}
///
/// \brief 求均值
///
void FunNum::mean()
{
    SAAbstractDatas* data = saUI->getSelectSingleData();
    if(nullptr == data)
    {
        return;
    }
    std::shared_ptr<SAVariantDatas> res = saFun::mean(data);
    if(nullptr == res)
    {
        saUI->showErrorMessageInfo(saFun::getLastErrorString());
        saUI->raiseMessageInfoDock();
        return;
    }
    res->setName(TR("%1-mean").arg(data->getName()));
    saValueManager->addData(res);
    saUI->showNormalMessageInfo(TR("%1 sum is %2").arg(data->getName()).arg(res->toData<double>()));
}
///
/// \brief 求和
///
void FunNum::sum()
{
    SAAbstractDatas* data = saUI->getSelectSingleData();
    if(nullptr == data)
    {
        return;
    }
    std::shared_ptr<SAVariantDatas> res = saFun::sum(data);
    if(nullptr == res)
    {
        saUI->showErrorMessageInfo(saFun::getLastErrorString());
        saUI->raiseMessageInfoDock();
        return;
    }
    res->setName(TR("%1-sum").arg(data->getName()));
    saValueManager->addData(res);
    saUI->showNormalMessageInfo(TR("%1 sum is %2").arg(data->getName()).arg(res->toData<double>()));
}


///
/// \brief 频率统计
///
void FunNum::hist()
{
    SAAbstractDatas* data = saUI->getSelectSingleData();
    if(nullptr == data)
    {
        return;
    }
    const QString idHistCount = "histCount";
    const QString idIsPlot = "isPlot";
    SAPropertySetDialog dlg(saUI->getMainWindowPtr(),static_cast<SAPropertySetDialog::BrowserType>(SAGUIGlobalConfig::getDefaultPropertySetDialogType()));
    dlg.appendGroup(TR("property set"));
    dlg.appendIntProperty(idHistCount,TR("hist count")
                          ,1,std::numeric_limits<int>::max()
                          ,100,TR("set hist count"));
    dlg.appendGroup(TR("plot set"));
    dlg.appendBoolProperty(idIsPlot,TR("is plot")
                           ,true
                           ,TR("is plot after calc"));
    if(QDialog::Accepted != dlg.exec())
    {
        return;
    }
    auto res = saFun::hist(data,dlg.getDataByID<int>(idHistCount));
    if(nullptr == res)
    {
        saUI->showErrorMessageInfo(saFun::getLastErrorString());
        saUI->raiseMessageInfoDock();
        return;
    }
    saValueManager->addData(res);
    if(dlg.getDataByID<bool>(idIsPlot))
    {
        QMdiSubWindow* sub = saUI->createFigureWindow();
        SAFigureWindow* fig = saUI->getFigureWidgetFromMdiSubWindow(sub);
        SAChart2D* chart = fig->create2DPlot();
        if(chart)
        {
            chart->addHistogram(res.get());
        }
        saUI->raiseMainDock();
        sub->show();
    }
}




