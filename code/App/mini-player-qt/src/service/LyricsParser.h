// LyricsParser：LRC 歌词解析器 + 二分定位。
// 对应书中 §07 阶段⑥ Step 7.1。
//
// LRC 格式：
//   [00:12.34]歌词文本
//   [00:12.34][00:30.00]重复行（支持一行多时间戳）
//   [ti:曲名][ar:艺术家][offset:+200]  元数据头
#pragma once

#include <QString>
#include <QVector>
#include <QHash>

namespace mp {

struct LyricLine {
    qint64 timeMs = 0;
    QString text;
};

struct LyricsDocument {
    QVector<LyricLine> lines;        // 按时间升序
    QHash<QString, QString> meta;    // ti/ar/al/by/offset...
    qint64 offsetMs = 0;             // [offset:xxx] 全局偏移
};

class LyricsParser {
public:
    /** 把 LRC 文本解析为文档；无效行安静跳过 */
    static LyricsDocument parse(const QString& lrcText);

    /** 二分查找 posMs 对应的当前行索引（返回 -1 表示还没开始） */
    static int findLineIndex(const QVector<LyricLine>& lines, qint64 posMs);
};

} // namespace mp
