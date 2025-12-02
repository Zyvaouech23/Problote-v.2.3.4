#pragma once
#include <QString>

class Note {
public:
    Note() = default;

    Note(const QString& subject,
        double value,
        double coefficient,
        const QString& date,
        const QString& type,
        const QString& comment)
        : m_subject(subject),
        m_value(value),
        m_coefficient(coefficient),
        m_date(date),
        m_type(type),
        m_comment(comment) {
    }

    const QString& subject() const { return m_subject; }
    double value() const { return m_value; }
    double coefficient() const { return m_coefficient; }
    const QString& date() const { return m_date; }
    const QString& type() const { return m_type; }
    const QString& comment() const { return m_comment; }

    void setSubject(const QString& s) { m_subject = s; }
    void setValue(double v) { m_value = v; }
    void setCoefficient(double c) { m_coefficient = c; }
    void setDate(const QString& d) { m_date = d; }
    void setType(const QString& t) { m_type = t; }
    void setComment(const QString& c) { m_comment = c; }

private:
    QString m_subject;
    double  m_value = 0.0;
    double  m_coefficient = 1.0;
    QString m_date;
    QString m_type;
    QString m_comment;
};


