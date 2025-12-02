#pragma once
#include <QString>

class Devoir {
public:
    Devoir() = default;

    Devoir(const QString& subject,
        const QString& title,
        const QString& dueDate,
        const QString& description,
        const QString& assignedDate)
        : m_subject(subject),
        m_title(title),
        m_dueDate(dueDate),
        m_description(description),
        m_assignedDate(assignedDate) {
    }

    const QString& subject() const { return m_subject; }
    const QString& title() const { return m_title; }
    const QString& dueDate() const { return m_dueDate; }
    const QString& description() const { return m_description; }
    const QString& assignedDate() const { return m_assignedDate; }

    void setSubject(const QString& s) { m_subject = s; }
    void setTitle(const QString& t) { m_title = t; }
    void setDueDate(const QString& d) { m_dueDate = d; }
    void setDescription(const QString& d) { m_description = d; }
    void setAssignedDate(const QString& d) { m_assignedDate = d; }

private:
    QString m_subject;
    QString m_title;
    QString m_dueDate;
    QString m_description;
    QString m_assignedDate;
};

