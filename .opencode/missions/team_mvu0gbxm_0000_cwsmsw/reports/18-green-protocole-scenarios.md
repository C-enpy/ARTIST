# 18 — Protocole scenarios GREEN, producteurs source autorises

Le lead a relu protocol.py au candidat 5f9c798bfe8850a8ad592504f614a505b16d04c6 puis execute les 17 tests synthetiques inchanges : 17/17, code0, aucune erreur/echec/skip. Empreintes du test et du protocole stables pendant le run. RED semantique precedent : b38c349, 84 echecs d'assertion/sous-cas observes avant ajout des validations.

Les controles portent sur les selections et arguments, les identites/schema JSON, operations updates/draws, populations et samples de temps mural cote CPU. Un verdict FAIL est conserve comme resultat de domaine, mais ne peut porter de mesure VALID ; ABSENT n'est pas zero. Ni identite de fichier ni renderer reel ne sont qualifies par ce parseur en memoire.

Preuves completes : out/verification-synthetic/scenario-protocol-green-2026-10-29T23-17-11-139Z/. Auteur uniform-scenarios ; runner lead, Python3.13.1 Windows. Commande : C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_scenarios -p test_*.py -v.

La suite source C++ locale est autorisee dans l'ownership deja ratifie : producteurs, CMake local optionnel et selections CTest precises pour le futur raccordement. Aucune compilation, execution native ou graphique n'est autorisee. Une revue independante sensible sera organisee sur ce livrable avant reception. Le lead a souligne les risques de JSON/exclusivite, bornes, destruction/contexte, effacement de cellules et classification materielle avant rendu.

Les interfaces du protocole ont ete transmises au coeur pour eviter un second schema incompatible. Cette transmission n'est pas une integration de branche : les consommateurs restent soumis aux vraies barrieres. Aucun gain ni fonctionnement moteur n'est annonce.
