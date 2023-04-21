/**
 * \file Dictionnaire.cpp
 * \brief Ce fichier contient une implantation des méthodes de la classe Dictionnaire
 * \author IFT-2008, Étudiant(e)
 * \version 0.1
 * \date avril 2023
 *
 */

#include "Dictionnaire.h"

// Limite du nombre de suggestions
#define LIMITE_SUGGESTIONS 5

namespace TP3
{
    /**
    * \fn Dictionnaire::Dictionnaire()
    * \brief Constructeur par défaut de la classe Dictionnaire
    * \post Un objet Dictionnaire vide est créé
    */
    Dictionnaire::Dictionnaire() : racine(nullptr), cpt(0) {}

    /**
     * \fn Dictionnaire::Dictionnaire(std::ifstream &fichier)
     * \brief Constructeur de la classe Dictionnaire à partir d'un fichier
     * \pre Le fichier doit être ouvert au préalable
     * \post Un objet Dictionnaire est créé à partir du fichier
     */
	Dictionnaire::Dictionnaire(std::ifstream &fichier): racine(nullptr), cpt(0)
    {
        if (fichier)
        {
            for( std::string ligneDico; getline( fichier, ligneDico); )
            {
                if (ligneDico[0] != '#') //Élimine les lignes d'en-tête
                {
                    // Le mot anglais est avant la tabulation (\t).
                    std::string motAnglais = ligneDico.substr(0,ligneDico.find_first_of('\t'));
                    
                    // Le reste (définition) est après la tabulation (\t).
                    std::string motTraduit = ligneDico.substr(motAnglais.length()+1, ligneDico.length()-1);

                    //On élimine tout ce qui est entre crochets [] (possibilité de 2 ou plus)
                    std::size_t pos = motTraduit.find_first_of('[');
                    while (pos!=std::string::npos)
                    {
                        std::size_t longueur_crochet = motTraduit.find_first_of(']')-pos+1;
                        motTraduit.replace(pos, longueur_crochet, "");
                        pos = motTraduit.find_first_of('[');
                    }
                    
                    //On élimine tout ce qui est entre deux parenthèses () (possibilité de 2 ou plus)
                    pos = motTraduit.find_first_of('(');
                    while (pos!=std::string::npos)
                    {
                        std::size_t longueur_crochet = motTraduit.find_first_of(')')-pos+1;
                        motTraduit.replace(pos, longueur_crochet, "");
                        pos = motTraduit.find_first_of('(');
                    }

                    //Position d'un tilde, s'il y a lieu
                    std::size_t posT = motTraduit.find_first_of('~');
                    
                    //Position d'un tilde, s'il y a lieu
                    std::size_t posD = motTraduit.find_first_of(':');
                    
                    if (posD < posT)
                    {
                        //Quand le ':' est avant le '~', le mot français précède le ':'
                        motTraduit = motTraduit.substr(0, posD);
                    }
                    
                    else
                    {
                        //Quand le ':' est après le '~', le mot français suit le ':'
                        if (posT < posD)
                        {
                            motTraduit = motTraduit.substr(posD, motTraduit.find_first_of("([,;\n", posD));
                        }
                        else
                        {
                            //Quand il n'y a ni ':' ni '~', on extrait simplement ce qu'il y a avant un caractère de limite
                            motTraduit = motTraduit.substr(0, motTraduit.find_first_of("([,;\n"));
                        }
                    }
                    
                    //On ajoute le mot au dictionnaire
                    ajouteMot(motAnglais, motTraduit);
                    //std::cout<<motAnglais << " - " << motTraduit<<std::endl;
                }
            }
        }
	}

    /**
     * \fn Dictionnaire::~Dictionnaire()
     * \brief Destructeur de la classe Dictionnaire
     * \pre Le dictionnaire doit exister
     * \post Le dictionnaire est détruit
     */
    Dictionnaire::~Dictionnaire()
    {
        _detruireDictionnaire(racine);
    }

    /**
     * \fn void Dictionnaire::ajouteMot(const std::string& motOriginal, const std::string& motTraduit)
     * \brief Ajoute un mot au dictionnaire et l'une de ses traductions en équilibrant l'arbre AVL
     * \param[in] motOriginal Le mot original
     * \param[in] motTraduit Le mot traduit
     * \post La traduction (et le mot original si absent) est ajoutée au dictionnaire
     * \post L'arbre AVL est équilibré
     */
    void Dictionnaire::ajouteMot(const std::string& motOriginal, const std::string& motTraduit)
    {
        _ajouteMot(racine, motOriginal, motTraduit);
    }

    /**
     * \fn void Dictionnaire::supprimeMot(const std::string& motOriginal)
     * \brief Supprime un mot et ses traductions du dictionnaire en équilibrant l'arbre AVL
     * \param[in] motOriginal Le mot original à supprimer
     * \pre Le mot doit exister dans le dictionnaire
     * \pre Le dictionnaire ne doit pas être vide
     * \post Le mot et ses traductions sont supprimés du dictionnaire
     * \post L'arbre AVL est équilibré
     * \exception logic_error Si le mot n'existe pas dans le dictionnaire
     * \exception logic_error Si le dictionnaire est vide
     */
    void Dictionnaire::supprimeMot(const std ::string& motOriginal)
    {
        _supprimeMot(racine, motOriginal);
    }

    /**
     * \fn double Dictionnaire::similitude(const std::string& mot1, const std::string& mot2)
     * \brief Calcule la similitude entre 2 mots (dans le dictionnaire ou pas) en utilisant l'algorithme de distance de Levenshtein
     * \param[in] mot1 Le premier mot
     * \param[in] mot2 Le deuxième mot
     * \return La similitude entre les deux mots (entre 0 et 1)
     */
    double Dictionnaire::similitude(const std::string& mot1, const std::string& mot2)
    {
        // On utilise l'algorithme de distance de Levenshtein
        // https://en.wikibooks.org/wiki/Algorithm_Implementation/Strings/Levenshtein_distance#C++
        const std::size_t len1 = mot1.size(), len2 = mot2.size();
        std::vector<std::vector<unsigned int>> d(len1 + 1, std::vector<unsigned int>(len2 + 1));

        d[0][0] = 0;
        for(unsigned int i = 1; i <= len1; ++i) d[i][0] = i;
        for(unsigned int i = 1; i <= len2; ++i) d[0][i] = i;

        for(unsigned int i = 1; i <= len1; ++i)
            for(unsigned int j = 1; j <= len2; ++j)
                        // note that std::min({arg1, arg2, arg3}) works only in C++11,
                        // for C++98 use std::min(std::min(arg1, arg2), arg3)
                        d[i][j] = std::min({ d[i - 1][j] + 1, d[i][j - 1] + 1, d[i - 1][j - 1] + (mot1[i - 1] == mot2[j - 1] ? 0 : 1) });
        
        // Ajout par rapport au site web. Changement de la distance en similitude
        int dist = d[len1][len2];
        int maxlen = (len1 > len2) ? len1 : len2;
        return 1.0 - (double)dist / (double)maxlen;
    }

    /**
     * \fn std::vector<std::string> Dictionnaire::suggereCorrections(const std::string& motMalEcrit)
     * \brief Suggère jusqu'à 5 corrections pour un mot mal écrit
     * \param[in] motMalEcrit Le mot mal écrit
     * \return Un vecteur de chaînes de caractères contenant les suggestions de corrections
     * \post Si le mot mal écrit existe dans le dictionnaire, le vecteur retourné est vide
     * \post Si le mot mal écrit n'existe pas dans le dictionnaire, le vecteur retourné contient les suggestions de corrections
     */
    std::vector<std::string> Dictionnaire::suggereCorrections(const std ::string& motMalEcrit)
    {
        std::vector<std::string> suggestions;
        if (appartient(motMalEcrit)) return suggestions;

        _suggereCorrections(racine, motMalEcrit, suggestions);
        if (suggestions.size() > LIMITE_SUGGESTIONS)
        {
            suggestions.erase(suggestions.begin() + LIMITE_SUGGESTIONS, suggestions.end());
        }
        return suggestions;
    }

    /**
     * \fn std::vector<std::string> Dictionnaire::traduit(const std::string& mot)
     * \brief Retourne les traductions possibles d'un mot
     * \param[in] mot Le mot à traduire
     * \return Un vecteur de chaînes de caractères contenant les traductions possibles du mot
     * \post Si le mot n'existe pas dans le dictionnaire, le vecteur retourné est vide
     * \post Si le mot existe dans le dictionnaire, le vecteur retourné contient les traductions présentes dans le dictionnaire
     */
    std::vector<std::string> Dictionnaire::traduit(const std ::string& mot)
    {
        //Trouver les traductions possibles d'un mot
	    //Si le mot appartient au dictionnaire, on retourne le vecteur des traductions du mot donné.
	    //Sinon, on retourne un vecteur vide
        std::vector<std::string> traductions;
        NoeudDictionnaire* noeud = _accedeMot(racine, mot);
        if (noeud != nullptr)
        {
            traductions = noeud->traductions;
        }
        return traductions;
    }

    /**
     * \fn bool Dictionnaire::appartient(const std::string& mot)
     * \brief Vérifie si un mot appartient au dictionnaire
     * \param[in] mot Le mot à vérifier
     * \return true si le mot appartient au dictionnaire, false sinon
     */
    bool Dictionnaire::appartient(const std::string &mot)
    {
        return _accedeMot(racine, mot) != nullptr;
    }

    /**
     * \fn bool Dictionnaire::estVide() const
     * \brief Vérifie si le dictionnaire est vide
     * \return true si le dictionnaire est vide, false sinon
     */
    bool Dictionnaire::estVide() const
    {
	    return this->cpt == 0;
    }

    /**
     * \fn unsigned int Dictionnaire::taille() const
     * \brief Retourne le nombre de mots dans le dictionnaire
     * \return Le nombre de mots dans le dictionnaire
     */
    unsigned int Dictionnaire::taille() const
    {
        return this->cpt;
    }

    /**
     * \fn bool Dictionnaire::estEquilibre() const
     * \brief Vérifie si le dictionnaire est équilibré
     * \return true si le dictionnaire est équilibré, false sinon
     */
    bool Dictionnaire::estEquilibre() const
    {
        return _estEquilibre(this->racine);
    }

	// Complétez ici l'implémentation avec vos méthodes privées.
    
    /**
     * \fn void Dictionnaire::_detruireDictionnaire(NoeudDictionnaire * &arbre)
     * \brief Méthode auxiliaire au destructeur pour détruire récursivement le dictionnaire
     * \param[in] arbre Le sous-arbre à détruire récursivement
     * \post Le noeud et ses sous-arbres sont détruits
     */
    void Dictionnaire::_detruireDictionnaire(NoeudDictionnaire * &arbre)
    {
        if (arbre != nullptr)
        {
            _detruireDictionnaire(arbre->gauche);
            _detruireDictionnaire(arbre->droite);
            delete arbre;
            arbre = nullptr;
        }
    }

    /**
     * \fn bool Dictionnaire::_ajouteMot(NoeudDictionnaire * &arbre, const std::string &motOriginal, const std::string &motTraduit)
     * \brief Méthode auxiliaire à ajouteMot pour ajouter un mot au dictionnaire par récursivité
     * \param[in] arbre Le noeud à ajouter
     * \param[in] motOriginal Le mot original à ajouter
     * \param[in] motTraduit Le mot traduit à ajouter
     * \return true si un nouveau mot a été ajouté, false sinon
     */
    bool Dictionnaire::_ajouteMot(NoeudDictionnaire * &arbre, const std::string &motOriginal, const std::string &motTraduit)
    {   
        // Indique si un mot, et pas seulement une traduction, a été ajouté
        // Permet de ne pas équilibrer l'arbre si c'est seulement une traduction qui a été ajoutée
        bool nvMotEstAjoute = false;

        if (arbre == nullptr)
        {
            arbre = new NoeudDictionnaire(motOriginal, motTraduit);
            cpt++;
            return true; // Se propage aux 'nvMotEstAjoute' jusqu'à la racine
        }
        
        if (motOriginal < arbre->mot)
        {
            nvMotEstAjoute = _ajouteMot(arbre->gauche, motOriginal, motTraduit);
        }
        else if (motOriginal > arbre->mot)
        {
            nvMotEstAjoute = _ajouteMot(arbre->droite, motOriginal, motTraduit);
        }
        else // Le mot existe déjà
        {
            if (!_traductionEstPresente(arbre, motTraduit)) arbre->traductions.push_back(motTraduit);
            return false; // Se propage aux 'nvMotEstAjoute' jusqu'à la racine
        }

        // Si un nouveau mot a été ajouté, on rééquilibre l'arbre et on met à jour les hauteurs
        if (nvMotEstAjoute) _equilibreAVL(arbre);
        return nvMotEstAjoute;
    }

    /**
     * \fn bool Dictionnaire::_traductionEstPresente(NoeudDictionnaire * &noeud, const std::string &motTraduit) const
     * \brief Méthode auxiliaire à ajouteMot pour savoir si un mot a déjà une traduction donnée
     * \param[in] noeud Le noeud à vérifier
     * \param[in] motTraduit Le mot traduit à vérifier
     * \return true si le mot a déjà la traduction donnée, false sinon
     * \pre Le noeud existe
     */
    bool Dictionnaire::_traductionEstPresente(NoeudDictionnaire * &noeud, const std::string &motTraduit) const
    {   
        if (noeud == nullptr) throw std::logic_error("Le noeud n'existe pas");
        else
        {
            for (int i = 0; i < noeud->traductions.size(); i++)
            {
                if (noeud->traductions[i] == motTraduit) return true;
            }
            return false;
        }
    }

    /**
     * \fn void Dictionnaire::_supprimeMot(NoeudDictionnaire * &arbre, const std::string &motOriginal)
     * \brief Méthode auxiliaire à supprimeMot pour supprimer un mot du dictionnaire par récursivité
     * \param[in] arbre Le sous-arbre dans lequel chercher le mot
     * \param[in] motOriginal Le mot original à supprimer
     * \pre Le dictionnaire n'est pas vide
     * \pre Le mot existe dans le dictionnaire
     * \post Le mot est supprimé du dictionnaire
     * \exception logic_error Le dictionnaire est vide
     * \exception logic_error Le mot n'existe pas dans le dictionnaire
     */
    void Dictionnaire::_supprimeMot(NoeudDictionnaire * &arbre, const std::string &motOriginal)
    {
        if (estVide())
        {
            throw std::logic_error("Le dictionnaire est vide");
        }
        if (arbre == nullptr)
        {
            throw std::logic_error("Le mot n'existe pas dans le dictionnaire");
        }

        if (motOriginal < arbre->mot) _supprimeMot(arbre->gauche, motOriginal);
        else if (motOriginal > arbre->mot) _supprimeMot(arbre->droite, motOriginal);

        // S'exécute si le mot est trouvé
        else if (arbre->gauche != nullptr && arbre->droite != nullptr)
        {
            // Cas complexe: le noeud a deux enfants. On remplace le mot par le plus petit mot de l'arbre droit
            _enleveMinDroite(arbre);
        }
        else
        {
            // Cas simple: le noeud a un seul enfant ou aucun enfant
            NoeudDictionnaire *temp = arbre;
            arbre = (arbre->gauche != nullptr) ? arbre->gauche : arbre->droite;
            delete temp;
            temp = nullptr;
            --cpt;
        }
        // On rééquilibre l'arbre et on met à jour les hauteurs
        _equilibreAVL(arbre);
    }

    /**
     * \fn void Dictionnaire::_enleveMinDroite(NoeudDictionnaire * &arbre)
     * \brief Méthode auxiliaire à supprimeMot pour supprimer un mot ayant deux enfants
     * \param[in] arbre Le noeud à supprimer
     * \pre Le noeud a deux enfants
     * \post Le noeud est supprimé du dictionnaire et remplacé par le plus petit mot de l'arbre droit
     */
    void Dictionnaire::_enleveMinDroite(NoeudDictionnaire * &arbre)
    {
        NoeudDictionnaire *temp = arbre->droite;
        NoeudDictionnaire *parent = arbre;
        while (temp->gauche != nullptr)
        {
            parent = temp;
            temp = temp->gauche;
        }
        arbre->mot = temp->mot; // On remplace le mot par le plus petit mot de l'arbre droit

        // On supprime le noeud (on a réduit au cas simple)
        if (temp == parent->gauche) _supprimeMot(parent->gauche, temp->mot);
        else _supprimeMot(parent->droite, temp->mot);
    }

    /**
     * \fn void Dictionnaire::_accedeMot(NoeudDictionnaire * &arbre, const std::string &data) const
     * \brief Méthode privée pour accéder à un mot. Est utilisée pour savoir si un mot est présent dans le dictionnaire
     * \param[in] arbre Le sous-arbre dans lequel chercher le mot
     * \param[in] data Le mot à chercher
     * \return Le noeud contenant le mot, nullptr si le mot n'est pas présent
     */
    Dictionnaire::NoeudDictionnaire* Dictionnaire::_accedeMot(NoeudDictionnaire * &arbre, const std::string &data) const
    {
        if (arbre == nullptr)
        {
            return nullptr;
        }
        if (data == arbre->mot)
        {
            return arbre;
        }
        else if (data < arbre->mot)
        {
            return _accedeMot(arbre->gauche, data);
        }
        else if (data > arbre->mot)
        {
            return _accedeMot(arbre->droite, data);
        }
        // Sécurité, ne devrait jamais arriver
        else
        {
            return nullptr;
        }
    }

    /**
     * \fn void Dictionnaire::_suggereCorrections(NoeudDictionnaire* const &arbre, const std::string &motMalEcrit, std::vector<std::string> &suggestions)
     * \brief Méthode auxiliaire de suggereCorrections pour chercher récursivement les mots similaires. Prend un vecteur de suggestions par référence pour le remplir
     * \param[in] arbre Le sous-arbre dans lequel chercher les mots
     * \param[in] motMalEcrit Le mot mal écrit
     * \param[in] suggestions Le vecteur de suggestions
     */
    void Dictionnaire::_suggereCorrections(NoeudDictionnaire* const &arbre, const std::string &motMalEcrit, std::vector<std::string> &suggestions)
    {
        if (arbre == nullptr) return;
        _suggereCorrections(arbre->gauche, motMalEcrit, suggestions);
        if (similitude(motMalEcrit, arbre->mot) >= 0.5) suggestions.push_back(arbre->mot);
        _suggereCorrections(arbre->droite, motMalEcrit, suggestions);
        /* if (similitude(motMalEcrit, arbre->mot) >= 0.5)
        {
            auto position = std::upper_bound(suggestions.begin(), suggestions.end(), arbre->mot std::greater<int>());
            suggestions.insert(position, arbre->mot);
            if (suggestions.size() > LIMITE_SUGGESTIONS) suggestions.pop_back();
        } */
    }

    /**
     * \fn void Dictionnaire::_hauteur(NoeudDictionnaire * &arbre) const
     * \brief Méthode privée pour calculer la hauteur d'un sous-arbre
     * \param[in] arbre Le sous-arbre
     * \return La hauteur du sous-arbre (ou -1 si le sous-arbre est vide)
     */
    int Dictionnaire::_hauteur(NoeudDictionnaire * &arbre) const
    {
        return (arbre == nullptr) ? -1 : arbre->hauteur; // la hauteur du vide = -1
    }

    /**
     * \fn void Dictionnaire::_estEquilibre(NoeudDictionnaire* const &arbre) const
     * \brief Méthode récursive auxiliaire à estEquilibre pour savoir si un sous-arbre est équilibré
     * \param[in] arbre Le sous-arbre
     * \return true si le sous-arbre est équilibré, false sinon
     */
    bool Dictionnaire::_estEquilibre(NoeudDictionnaire* const &arbre) const
    {
        // https://www.geeksforgeeks.org/how-to-determine-if-a-binary-tree-is-balanced/
        // for height of left subtree
        int lh;
    
        // for height of right subtree
        int rh;
    
        // If tree is empty then return true
        if (arbre == nullptr)
        {
            return true;
        }
        // Get the height of left and right sub trees
        lh = _hauteur(arbre->gauche);
        rh = _hauteur(arbre->droite);
    
        if (abs(lh - rh) <= 1 && _estEquilibre(arbre->gauche) && _estEquilibre(arbre->droite))
        {
            return true;
        }
        // If we reach here then tree is not height-balanced
        return false;
    }

    /**
     * \fn void Dictionnaire::_equilibreAVL(NoeudDictionnaire * &arbre)
     * \brief Méthode privée pour équilibrer un sous-arbre et mettre à jour sa hauteur. Utilisé lors de la remontée par les méthodes récursives
     * \param[in] arbre Le sous-arbre à équilibrer
     * \post Le sous-arbre est équilibré
     */
    void Dictionnaire::_equilibreAVL(NoeudDictionnaire * &arbre)
    {
        if (arbre == nullptr) return;
        if (_hauteur(arbre->gauche) - _hauteur(arbre->droite) > 1)
        {
            // Lorsque le débalancement est à gauche,
            // on fait un zigZig lorsque le sousArbre penche à gauche OU est balancé.
            if (_hauteur(arbre->gauche->gauche) >= _hauteur(arbre->gauche->droite))
            {
                _zigZigGauche(arbre);
            }
            else
            {
                _zigZagGauche(arbre);
            }
        }
        else if (_hauteur(arbre->droite) - _hauteur(arbre->gauche) > 1)
        {
            // Lorsque le débalancement est à droite,
            // on fait un zigZig lorsque le sousArbre penche à droite OU est balancé.
            if (_hauteur(arbre->droite->droite) >= _hauteur(arbre->droite->gauche))
            {
                _zigZigDroit(arbre);
            }
            else
            {
                _zigZagDroit(arbre);
            }
        }
        else
        {
            // par opérateur ternaire pour éviter d'importer <algorithm>
            int max = (_hauteur(arbre->gauche) > _hauteur(arbre->droite)) ? _hauteur(arbre->gauche) : _hauteur(arbre->droite);
            arbre->hauteur = 1 + max;
        }
    }

    /**
     * \fn void Dictionnaire::_zigZigGauche(NoeudDictionnaire * &K2)
     * \brief Méthode auxiliaire à _equilibreAVL pour équilibrer un sous-arbre en zigZig à gauche et mettre à jour les hauteurs.
     * \param[in] K2 Le sous-arbre à équilibrer
     * \pre Le sous-arbre est en zigZig à gauche
     * \post Une rotation à droite est effectuée et les hauteurs sont mises à jour
     */
    void Dictionnaire::_zigZigGauche(NoeudDictionnaire * &K2)
    {
        NoeudDictionnaire *K1 = K2->gauche;
        //std::cout << "zigZigGauche: hauteur " << K1->mot << "=" << K1->hauteur << " hauteur " << K2->mot << "=" << K2->hauteur << std::endl;
        K2->gauche = K1->droite;
        K1->droite = K2;
        // par opérateur ternaire pour éviter d'importer <algorithm>
        int max_h_K2 = (_hauteur(K2->gauche) > _hauteur(K2->droite)) ? _hauteur(K2->gauche) : _hauteur(K2->droite);
        K2->hauteur = 1 + max_h_K2;
        int max_h_K1 = (_hauteur(K1->gauche) > K2->hauteur) ? _hauteur(K1->gauche) : K2->hauteur;
        K1->hauteur = 1 + max_h_K1;
        K2 = K1;
        //std::cout << "fin zigZigGauche: hauteur " << K1->mot << "=" << K1->hauteur << " hauteur " << K1->droite->mot << "=" << K1->droite->hauteur << std::endl;
    }

    /**
     * \fn void Dictionnaire::_zigZigDroit(NoeudDictionnaire * &K2)
     * \brief Méthode auxiliaire à _equilibreAVL pour équilibrer un sous-arbre en zigZig à droite et mettre à jour les hauteurs.
     * \param[in] K2 Le sous-arbre à équilibrer
     * \pre Le sous-arbre est en zigZig à droite
     * \post Une rotation à gauche est effectuée et les hauteurs sont mises à jour
     */
    void Dictionnaire::_zigZigDroit(NoeudDictionnaire * &K2)
    {
        NoeudDictionnaire *K1 = K2->droite;
        //std::cout << "zigZigDroit: hauteur " << K1->mot << "=" << K1->hauteur << " hauteur " << K2->mot << "=" << K2->hauteur << std::endl;
        K2->droite = K1->gauche;
        K1->gauche = K2;
        // par opérateur ternaire pour éviter d'importer <algorithm>
        int max_h_K2 = (_hauteur(K2->gauche) > _hauteur(K2->droite)) ? _hauteur(K2->gauche) : _hauteur(K2->droite);
        K2->hauteur = 1 + max_h_K2;
        int max_h_K1 = (_hauteur(K1->droite) > K2->hauteur) ? _hauteur(K1->droite) : K2->hauteur;
        K1->hauteur = 1 + max_h_K1;
        K2 = K1;
        //std::cout << "fin zigZigDroit: hauteur " << K1->mot << "=" << K1->hauteur << " hauteur " << K1->gauche->mot << "=" << K1->gauche->hauteur << std::endl;
    }

    /**
     * \fn void Dictionnaire::_zigZagGauche(NoeudDictionnaire * &K3)
     * \brief Méthode auxiliaire à _equilibreAVL pour équilibrer un sous-arbre en zigZag à gauche et mettre à jour les hauteurs.
     * \param[in] K3 Le sous-arbre à équilibrer
     * \pre Le sous-arbre est en zigZag à gauche
     * \post Le sous-arbre est équilibré et ses hauteurs sont mises à jour
     */
    void Dictionnaire::_zigZagGauche(NoeudDictionnaire * &K3)
    {
        _zigZigDroit(K3->gauche);
        _zigZigGauche(K3);
    }

    /**
     * \fn void Dictionnaire::_zigZagDroit(NoeudDictionnaire * &K3)
     * \brief Méthode auxiliaire à _equilibreAVL pour équilibrer un sous-arbre en zigZag à droite et mettre à jour les hauteurs.
     * \param[in] K3 Le sous-arbre à équilibrer
     * \pre Le sous-arbre est en zigZag à droite
     * \post Le sous-arbre est équilibré et ses hauteurs sont mises à jour
     */
    void Dictionnaire::_zigZagDroit(NoeudDictionnaire * &K3)
    {
        _zigZigGauche(K3->droite);
        _zigZigDroit(K3);
    }

}//Fin du namespace
