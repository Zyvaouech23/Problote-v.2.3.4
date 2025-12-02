#pragma once
#include <QString>

class Matiere {
public:
    Matiere() = default;

    Matiere(const QString& name, double average, double coefficientSum = 0.0)
        : m_name(name),
        m_average(average),
        m_coefficientSum(coefficientSum) {
    }

    const QString& name() const { return m_name; }
    double average() const { return m_average; }
    double coefficientSum() const { return m_coefficientSum; }

    void setName(const QString& n) { m_name = n; }
    void setAverage(double a) { m_average = a; }
    void setCoefficientSum(double c) { m_coefficientSum = c; }

private:
    QString m_name;
    double  m_average = 0.0;
    double  m_coefficientSum = 0.0;
};

