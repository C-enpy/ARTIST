# 47 — C-OS : refus documenté, récupération à la frontière processus à examiner

2026-10-26. Trois fragments C_OS/FIN_C_OS reçus, auteur b3-contract/architecte en contexte conservé non indépendant autorisé46. HEADefd4bb4 lu ; sources42/43/44/46/QA17. Aucun code/flag/handle/processus d'essai créé ou modifié, aucune commande de fermeture exécutée. Tâche C-OS reste in_progress avant complément/intégration.

## Sources officielles et recoupement lead

Lectures Microsoft de l'agent, puis fetch/index et recherches ciblées lead réussis sur les mêmes quatre pages le26octobre :
- A https://learn.microsoft.com/en-us/windows/win32/api/handleapi/nf-handleapi-sethandleinformation
- B https://learn.microsoft.com/en-us/windows/win32/api/handleapi/nf-handleapi-closehandle
- C https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/get-osfhandle?view=msvc-170
- D https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/fclose-fcloseall?view=msvc-170

A : file est type supporté, protection0x2 empêche CloseHandle de fermer, masque sélectionne le bit/flags0 le retire. B : handle valide, retour0/GetLastError en échec ; exception debugger documentée pour invalide/pseudo-handle, pas preuve d'absence d'exception avec protection. Aucun code numérique précis du refus protégé n'est garanti par ces pages.

Le flag ne rend pas le handle invalide : condition de refus OS plausible, pas throw/EOF/erreurCRT injectés par mock ; mais refus délibérément configuré par test, pas panne spontanée de stockage. Étude46 n'autorise ni son application ni son admissibilité à l'obligation M5.

## Propagation conditionnelle et ownership

Si CloseHandle retourneFALSE normalement sur handle protégé : sourceCRT42 remonte erreur, dissocie fd, libère FILE*/buffer et retourneEOF. Source installée ≠ DLLchargée certifiée ; errno précis/absence d'exception non acquis.

C avertit expressément de ne jamais CloseHandle le handle emprunté via_get_osfhandle ; ownerfd, lui-même ownerFILE*. D interdit doublefermeture additionnelle. Après fclose consommé, aucune réutilisation FILE*/fd/_get_osfhandle(fd), fd recyclable. Copier la valeur du handle n'attribue ni ownership ni génération. Sous le refus supposé, risque de handle encore ouvert après ownersCRT consommés ; retraitflag+CloseHandle direct sur valeur empruntée n'est **pas** établi sûr/public. Déprotéger avant fclose supprime le négatif ; duplication ne transfère pas magiquement l'ownership original.

La remise C_OS conclut donc : candidat OS documenté, propagationCRT conditionnelle, récupération publique sûre non établie dans les seules A–D. Aucune commande expérimentale adoptable à ce stade, ni impossibilité universelle. Une calibration FILE*contrôlé ne prouverait pas accès au ifstream privé, non-publicationReader ou catch. Syscall/CRT/publication sont trois preuves distinctes.

## Apport nouveau lead, même hypothèse : terminaison du processus possédé

Avant intégration finale, lecture officielle complémentaire réussie : https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-exitprocess . Remarks décrit fermeture de **tous les handles ouverts par le processus** à l'étape6 et indique que retour de main appelle ExitProcess. Protection décrite parA vise CloseHandle ; la récupération proposée n'appelle pas CloseHandle sur l'emprunt.

Proposition documentaire envoyée à C pour complément ciblé de la **même hypothèse** : probe dans enfant possédé/dédié, une fclose négative et FILE*consommé, aucun retryfd/FILE/CloseHandle emprunté ; retour normal/fin de cet enfant, parent attend terminaison puis vérifie fixture/cleanup/voisins. Aucun enfant lancé ni flag appliqué. L'agent doit évaluer contrat public et risques detach/timeout/ownership ; pas présumer un dispositif sûr ou une preuve exécutée. La source décrit aussi risque de deadlock au detach si threads/verrous : protocole monothread/finalisation bornée et constat de terminaison sont préconditions à étudier, pas déjà démontrées.

## Portes restantes avant toute expérience

Admissibilité de l'injection de politique OS explicitement acceptée ; cleanup public sûr ou dérogation motivée explicite ; identitéCRTchargée/chemin recoupé ; retour/exception debugger traité ; fixture/lecture/flag/fclose/récupération/oracles distincts définis et commandes exactes approuvées. Pas de seam moteur ni adoptionReader/catch/messages/mode. M5/F8 non prouvés ; statuts ADR inchangés.
