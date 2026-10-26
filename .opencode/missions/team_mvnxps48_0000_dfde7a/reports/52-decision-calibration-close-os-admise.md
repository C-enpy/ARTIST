# 52 — Calibration close OS ciblée admise explicitement

2026-10-26. Réponse opérateur après checkpoint51 : **Admettre calibration**.

Admission ciblée du refus natif délibérément provoqué par HANDLE_FLAG_PROTECT_FROM_CLOSE sur le handle valide d'un **probe FILE* en enfant dédié** ; instrumentation de cet emprunt permise à cette seule fin, sans transfert d'ownership ni fermeture manuelle du handle emprunté. Récupération proposée par terminaison normale de l'enfant possédé, parent constate signal de terminaison avant cleanup. Ce choix lève l'interdiction des flags fabriquant preuve uniquement pour cette injection de politiqueOS précise, pas pour EOF/ferror/throw/handle invalidé.

**Limite acceptée** : calibration native/CRT partielle, jamais M5Reader/non-publication/catch couverts par assimilation. Reader/modes/messages/seam/accès au ifstream privé non adoptés ; aucune correction moteur autorisée.

Conditions avant code/essais : contrat de probe/protocole et amendement QA commis, auteur fresh et réellement isolé, identitéCRT exécutée et comportement retour/exception à observer, commandes exactes approuvées avant runnerlead. Supervision/timeout/cleanup doivent être explicitement définis ; timeout ne prouve pas terminaison, aucun TerminateProcess fallback adopté. Aucun processus/flag/handle encore manipulé ni test/build exécuté.

Suite : préparation PM et amendement QA ciblés en parallèle, sans écrivain partagé, pendant la tranche source REDstdio ; publication d'un auteur close seulement après entrées intégrées et admission native. Scores développement≥70, routage politique inchangé, aucun nouveau provider/modèle/droit/service/dépendance. La préparation peut conserver les contextes PM/QA propres à cette mission, explicitement non indépendants ; futur auteur/reviewer fresh inchangés.
