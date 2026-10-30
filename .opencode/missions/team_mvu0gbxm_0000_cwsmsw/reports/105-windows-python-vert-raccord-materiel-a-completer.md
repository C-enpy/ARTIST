# 105 — Adaptateur Windows vert, déclaration matérielle à compléter

Le coordinateur a relu le candidat et vérifié les tests inchangés ainsi que
l'identité du corps PowerShell après le bloc de paramètres. Le helper reste fermé.
Résultats : **47/47 tests Windows**, cœur **191 succès et trois SKIP sur194**,
contexte **28/28**, scénarios **31/31**, aucun échec ou erreur, empreintes stables.
Les SKIP sont les liens symboliques sans privilèges Windows ; aucun contournement.

La relecture identifie un raccord nécessaire au contrat commun déjà ratifié :
seule l'exécution CTest porte les six variables de transport matériel. Les
workloads et la découverte CTest n'en déclarent pas. Le futur nettoyage du runner,
volontairement déclenché seulement par déclaration explicite, ne s'y appliquerait
donc pas. Le parent peut encore y forcer un rendu logiciel. Aucun faux succès
matériel n'est démontré.

Une régression de description est demandée avant correction, sur les six valeurs
et identités par invocation workload/découverte/exécution, avec CPU inchangé.
Le nettoyage effectif reste uniquement au runner partagé ; aucune duplication dans
l'adaptateur, lecture d'environnement parent ou valeur vide artificielle. Les
47 tests et le helper restent inchangés pendant ce RED supplémentaire.

Le candidat n'est pas intégré. Les anciens contrôles des fragments PowerShell
n'ont pas été relancés à cette étape : leur corps/pins/tests sont inchangés ;
leurs preuves antérieures gardent leur portée, pas une nouvelle qualification.

## Preuves

- Candidat `84b9f80ca276665fdfb7e19126a82f664ca2c4bc`, auteur pool-23 ; runner coordinateur.
- Commande Windows : `C:/Python313/python.exe -B -m unittest -v test_adapter_binding test_binding_helper_interface test_ctest_cache_binding`, cwd répertoire des tests Windows du worktree auteur.
- Non-régression : même interpréteur, `-B -m unittest discover -s scripts/tests/<suite> -p test_*.py -v`, suites core/contexte/scénarios depuis racine worktree.
- Manifestes/logs : `out/verification-synthetic/windows-python-binding-green-2026-10-30T07-11-03-266Z/`.
- Une erreur d'échappement du script Context Mode a été corrigée avant ces commandes ; ce premier script n'a exécuté aucun test et ne constitue pas un RED.

Aucun PowerShell, helper complet, outil natif, moteur, GL, rendu, benchmark,
installation ou cloud exécuté. Le cache admet des octets déclarant un chemin,
pas l'identité du binaire CTest lui-même ; qualification native NON_EXECUTE.
