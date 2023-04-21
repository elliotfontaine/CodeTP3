/**
 * \file Principal.cpp
 * \brief Fichier de chargement pour le dictionnaire, avec interface de traduction
 * \author IFT-2008, Étudiant(e)
 * \version 0.1
 * \date avril 2023
 *
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include "Dictionnaire.h"

using namespace std;
using namespace TP3;


/**
 * \brief Affiche un choix de mots à l'utilisateur parmi ceux d'un vecteur,
 * et retourne l'index du mot choisi, ou 0 si l'entrée est invalide.
 * \param[in] listeChoix Le vecteur de mots à afficher
 * \return L'index du mot choisi, ou 0 si l'entrée est invalide.
 */
size_t afficheListeChoix(const vector<string> &listeChoix)
{
	string reponse;
	size_t idx = 0;
	for (vector<string>::const_iterator j = listeChoix.begin(); j != listeChoix.end(); j++)
	{
		idx++;
		cout << idx << ". "<< *j << endl;
	}
	cout << "Votre choix : ";
	getline(cin, reponse);
	int choix = -1;
	try {
		choix = stoi(reponse);
	} catch (invalid_argument&) {
		// La réponse n'est pas un nombre valide
	} catch (out_of_range&) {
		// La réponse est hors de portée pour le type int
	}
	if (choix < 1 || choix > listeChoix.size())
	{
		cout << "Choix invalide. Celui par défaut (1.) sera pris." << endl;
		return 0;
	}
	else return choix - 1;
}

/**
 * \brief Fonction principale du programme. Charge le dictionnaire et permet à l'utilisateur de traduire une phrase.
 * \return 0 si le programme s'est terminé normalement, 1 sinon.
 */
int main()
{
	
	try
	{

		string reponse = "";
		ifstream englishFrench;

		bool reponse_lue = false; //Booléen pour savoir que l'utilisateur a saisi une réponse

		//Tant que la réponse est vide ou non valide, on redemande.
		while (!reponse_lue)
		{
			cout << "Entrez le nom du fichier du dictionnaire Anglais-Francais : ";
			getline(cin, reponse);
			englishFrench.open(reponse.c_str());
			if (englishFrench) {
				cout << "Fichier '" << reponse << "' lu!" << endl;
				reponse_lue = true;
			}
			else {
				cout << "Fichier '" << reponse << "' introuvable! Veuillez entrer un nom de fichier, ou son chemin absolu." << endl;
				cin.clear();
				// cin.ignore(); // sur MacOS cette ligne empeche de lire le 1e caractere pour le 2e essai ?!
			}
		}

		//Initialisation d'un ditionnaire, avec le constructeur qui utilise un fstream
		Dictionnaire dictEnFr(englishFrench);
		englishFrench.close();


	    // Affichage du dictionnaire niveau par niveau
		// Et vérification de l'équilibre de l'arbre
		// Et affichage du nombre de mots
	    cout << dictEnFr << endl;
		bool eq = dictEnFr.estEquilibre();
		cout << "Arbre équilibré : " << (eq ? "Oui" : "Non") << endl;
		cout << "Nombre de mots : " << dictEnFr.taille() << endl;
		cout << endl;

		vector<string> motsAnglais; //Vecteur qui contiendra les mots anglais de la phrase entrée

		//Lecture de la phrase en anglais
		cout << "Entrez un texte en anglais (pas de majuscules ou de ponctuation/caracteres speciaux):" << endl;
		getline(cin, reponse);

		stringstream reponse_ss(reponse);
		string str_temp;

		//On ajoute un mot au vecteur de mots tant qu'on en trouve dans la phrase (séparateur = espace)
		while (reponse_ss >> str_temp)
		{
			motsAnglais.push_back(str_temp);
		}

		vector<string> motsFrancais; //Vecteur qui contiendra les mots traduits en français

		for (vector<string>::iterator i = motsAnglais.begin(); i != motsAnglais.end(); i++)
			// Itération dans les mots anglais de la phrase donnée
			// CHOIX ETUDIANT: j'ai changé le const_iterator en iterator pour pouvoir modifier le vecteur
			// de mots anglais en cas de correction de mot
		{
			// À compléter ...
			// _________________________________
			// CODE ETUDIANT
			// _________________________________
			
			vector<string> traductions = dictEnFr.traduit(*i);
			
			if (traductions.size() == 0) 
			{
				// On commence par essayer de ramener le mot à l'un de ceux du dictionnaire
				vector<string> suggestions = dictEnFr.suggereCorrections(*i);
				if (suggestions.size() == 0)
				{
					// Le mot n'existe pas dans le dictionnaire, et aucune suggestion n'a été trouvée
					cout << "Le mot '" << *i << "' n'existe pas dans le dictionnaire. Veuillez entrer manuellement un mot de remplacement (ENTER pour ignorer):" << endl;
					cout << "Votre choix : ";
					getline(cin, reponse);
					motsFrancais.push_back(reponse);
				}
				else
				{
					// Le mot n'existe pas dans le dictionnaire, mais des suggestions ont été trouvées
					cout << "Le mot '" << *i << "' n'existe pas dans le dictionnaire. Veuillez choisir une des suggestions suivantes :" << endl;
					size_t choix = afficheListeChoix(suggestions);
					*i = suggestions[choix];
					traductions = dictEnFr.traduit(*i);
				}
			}

			if (traductions.size() == 1)
			{
				// Cas trivial, une seule traduction possible
				motsFrancais.push_back(traductions[0]);
			}
			else if (traductions.size() > 1)
			{
				// Plusieurs traductions possibles
				cout << "Plusieurs traductions sont possibles pour le mot '" << *i << "'. Veuillez en choisir une parmi les suivantes: " << endl;
				size_t choix = afficheListeChoix(traductions);
				motsFrancais.push_back(traductions[choix]);
			}
		}
		// _________________________________
		//  FIN CODE ETUDIANT
		// _________________________________

		stringstream phraseFrancais; // On crée un string contenant la phrase,
									 // À partir du vecteur de mots traduits.
		for (vector<string>::const_iterator i = motsFrancais.begin(); i != motsFrancais.end(); i++)
		{
			phraseFrancais << *i << " ";
		} 

		cout << "La phrase en francais est :" << endl << phraseFrancais.str() << endl;

	}
	catch (exception & e)
	{
		cerr << e.what() << endl;
	}
 
	return 0;
}
