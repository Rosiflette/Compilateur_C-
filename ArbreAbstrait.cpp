#include <stdlib.h>
#include "ArbreAbstrait.h"
#include "Symbole.h"
#include "SymboleValue.h"
#include "Exceptions.h"
#include <typeinfo>

////////////////////////////////////////////////////////////////////////////////
// NoeudSeqInst
////////////////////////////////////////////////////////////////////////////////

NoeudSeqInst::NoeudSeqInst() : m_instructions() {
}

int NoeudSeqInst::executer() {
    for (unsigned int i = 0; i < m_instructions.size(); i++)
        m_instructions[i]->executer(); // on exécute chaque instruction de la séquence
    return 0; // La valeur renvoyée ne représente rien !
}

void NoeudSeqInst::ajoute(Noeud* instruction) {
    if (instruction != nullptr) m_instructions.push_back(instruction);
}

////////////////////////////////////////////////////////////////////////////////
// NoeudAffectation
////////////////////////////////////////////////////////////////////////////////

NoeudAffectation::NoeudAffectation(Noeud* variable, Noeud* expression)
: m_variable(variable), m_expression(expression) {
}

int NoeudAffectation::executer() {
    int valeur = m_expression->executer(); // On exécute (évalue) l'expression
    ((SymboleValue*) m_variable)->setValeur(valeur); // On affecte la variable
    return 0; // La valeur renvoyée ne représente rien !
}

////////////////////////////////////////////////////////////////////////////////
// NoeudOperateurBinaire
////////////////////////////////////////////////////////////////////////////////

NoeudOperateurBinaire::NoeudOperateurBinaire(Symbole operateur, Noeud* operandeGauche, Noeud* operandeDroit)
: m_operateur(operateur), m_operandeGauche(operandeGauche), m_operandeDroit(operandeDroit) {
}

int NoeudOperateurBinaire::executer() {
    int og, od, valeur;
    if (m_operandeGauche != nullptr) og = m_operandeGauche->executer(); // On évalue l'opérande gauche
    if (m_operandeDroit != nullptr) od = m_operandeDroit->executer(); // On évalue l'opérande droit
    // Et on combine les deux opérandes en fonctions de l'opérateur
    if (this->m_operateur == "+") valeur = (og + od);
    else if (this->m_operateur == "-") valeur = (og - od);
    else if (this->m_operateur == "*") valeur = (og * od);
    else if (this->m_operateur == "==") valeur = (og == od);
    else if (this->m_operateur == "!=") valeur = (og != od);
    else if (this->m_operateur == "<") valeur = (og < od);
    else if (this->m_operateur == ">") valeur = (og > od);
    else if (this->m_operateur == "<=") valeur = (og <= od);
    else if (this->m_operateur == ">=") valeur = (og >= od);
    else if (this->m_operateur == "et") valeur = (og && od);
    else if (this->m_operateur == "ou") valeur = (og || od);
    else if (this->m_operateur == "non") valeur = (!og);
    else if (this->m_operateur == "/") {
        if (od == 0) throw DivParZeroException();
        valeur = og / od;
    }
    return valeur; // On retourne la valeur calculée
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstSi
////////////////////////////////////////////////////////////////////////////////

NoeudInstSi::NoeudInstSi(Noeud* condition, Noeud* sequence)
: m_condition(condition), m_sequence(sequence) {
}

int NoeudInstSi::executer() {
    if (m_condition->executer()) m_sequence->executer();
    return 0; // La valeur renvoyée ne représente rien !
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstSiRiche
////////////////////////////////////////////////////////////////////////////////

NoeudInstSiRiche::NoeudInstSiRiche(Noeud* condition, Noeud* sequence, vector<Noeud *> & conditionsSinon, vector<Noeud *> & sequencesSinon, Noeud* sequenceElse)
: m_conditionSi(condition), m_sequenceSi(sequence), m_conditions(conditionsSinon), m_sequences(sequencesSinon), m_sequenceElse(sequenceElse) {

}

NoeudInstSiRiche::NoeudInstSiRiche(Noeud* condition, Noeud* sequence, Noeud* sequenceElse)
: m_conditionSi(condition), m_sequenceSi(sequence), m_sequenceElse(sequenceElse) {

}

int NoeudInstSiRiche::executer() {
    bool condition = false;
    if (m_conditionSi->executer()) {
        condition = true;
        m_sequenceSi->executer();
    } else if (condition != true) {
        int i = 0;
        while (i < m_conditions.size() && condition != true) {
            if (m_conditions[i]->executer()) {
                condition = true;
                m_sequences[i]->executer();
            }
            i++;
        }
    } else {
        if (m_sequenceElse != nullptr) {
            m_sequenceElse->executer();
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstTq
////////////////////////////////////////////////////////////////////////////////

NoeudInstTq::NoeudInstTq(Noeud* condition, Noeud* sequence)
: m_condition(condition), m_sequence(sequence) {
}

int NoeudInstTq::executer() {
    while (m_condition->executer()) {
        m_sequence->executer();
    }
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstEcrire
////////////////////////////////////////////////////////////////////////////////

NoeudInstEcrire::NoeudInstEcrire() : m_donnees() {
    //    <instEcrire> ::= ecrire ( <expression> | <chaine> {, <expression> | <chaine>})
}

int NoeudInstEcrire::executer() {
    for (unsigned int i = 0; i < m_donnees.size(); i++) {
        // On regarde si l'objet pointé par m_donnees est de type SymboleValue et si c'est une chaine
        if ((typeid (m_donnees[i]) == typeid (SymboleValue))&& *((SymboleValue*) m_donnees[i]) == "<CHAINE>") {
            // on exécute chaque instruction d'écrire
            cout << m_donnees[i];
        } else {

            cout << m_donnees[i]->executer();
        }

    }
    return 0; // La valeur renvoyée ne représente rien !
}

void NoeudInstEcrire::ajoute(Noeud* instruction) {
    if (instruction != nullptr) m_donnees.push_back(instruction);
}


////////////////////////////////////////////////////////////////////////////////
// NoeudInstRepeter
////////////////////////////////////////////////////////////////////////////////

NoeudInstRepeter::NoeudInstRepeter(Noeud* instruction, Noeud* condition)
: m_seqInstru(instruction), m_express(condition) {

}

int NoeudInstRepeter::executer() {
    do{
        m_seqInstru->executer();
    } while(m_express->executer());
    return 0; // La valeur renvoyée ne représente rien !
}



////////////////////////////////////////////////////////////////////////////////
// NoeudInstPour
////////////////////////////////////////////////////////////////////////////////

NoeudInstPour::NoeudInstPour(Noeud* affec, Noeud* expression, Noeud * affect, Noeud* instru)
: m_affec(affec), m_express(expression), m_affect(affect), m_seqInstru(instru) {

}

int NoeudInstPour::executer() {
 ////////////////////A COMPLETER Changer nom variable..
    for(m_affec->executer();m_express->executer();m_affect->executer()){
        m_seqInstru->executer();
    }
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstLire
////////////////////////////////////////////////////////////////////////////////

NoeudInstLire::NoeudInstLire()
:
{
}

int NoeudInstLire::executer() {
    
}