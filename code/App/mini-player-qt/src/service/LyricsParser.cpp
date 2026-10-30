#include "LyricsParser.h"

#include <QRegularExpression>
#include <algorithm>

namespace mp {

LyricsDocument LyricsParser::parse(const QString& lrcText) {
    LyricsDocument doc;
    if (lrcText.isEmpty()) return doc;

    // 匹配时间戳 [mm:ss.xx] 或 [mm:ss]；一行可多个时间戳
    static const QRegularExpression kTimeRe(QStringLiteral(R"(\[(\d{1,2}):(\d{1,2})(?:[.:](\d{1,3}))?\])"));
    // 匹配元数据头 [ti:xxx]
    static const QRegularExpression kMetaRe(QStringLiteral(R"(^\s*\[([a-zA-Z]+):([^\]]*)\]\s*$)"));

    const QStringList rawLines = lrcText.split(QRegularExpression(QStringLiteral("\r?\n")));
    for (const QString& raw : rawLines) {
        const QString line = raw.trimmed();
        if (line.isEmpty()) continue;

        // 元数据头
        auto metaMatch = kMetaRe.match(line);
        if (metaMatch.hasMatch()) {
            const QString key = metaMatch.captured(1).toLower();
            const QString val = metaMatch.captured(2).trimmed();
            doc.meta[key] = val;
            if (key == QStringLiteral("offset")) {
                bool ok = false;
                const qint64 ms = val.toLongLong(&ok);
                if (ok) doc.offsetMs = ms;
            }
            continue;
        }

        // 时间戳 + 文本：收集所有时间戳
        QVector<qint64> stamps;
        auto it = kTimeRe.globalMatch(line);
        int lastEnd = 0;
        while (it.hasNext()) {
            auto m = it.next();
            const qint64 mm = m.captured(1).toLongLong();
            const qint64 ss = m.captured(2).toLongLong();
            const QString msStr = m.captured(3);
            qint64 ms = 0;
            if (!msStr.isEmpty()) {
                if (msStr.size() == 2)      ms = msStr.toLongLong() * 10;
                else if (msStr.size() == 3) ms = msStr.toLongLong();
                else                        ms = msStr.toLongLong();
            }
            stamps.push_back(mm * 60'000 + ss * 1'000 + ms);
            lastEnd = m.capturedEnd();
        }
        if (stamps.isEmpty()) continue;
        const QString text = line.mid(lastEnd).trimmed();
        for (qint64 t : stamps) {
            doc.lines.push_back(LyricLine{ t, text });
        }
    }

    std::sort(doc.lines.begin(), doc.lines.end(),
              [](const LyricLine& a, const LyricLine& b){ return a.timeMs < b.timeMs; });
    return doc;
}

int LyricsParser::findLineIndex(const QVector<LyricLine>& lines, qint64 posMs) {
    if (lines.isEmpty() || posMs < lines.first().timeMs) return -1;
    // 二分：最后一个 <= posMs 的
    int lo = 0, hi = lines.size() - 1, ans = -1;
    while (lo <= hi) {
        const int mid = (lo + hi) >> 1;
        if (lines[mid].timeMs <= posMs) { ans = mid; lo = mid + 1; }
        else                             hi = mid - 1;
    }
    return ans;
}

} // namespace mp
