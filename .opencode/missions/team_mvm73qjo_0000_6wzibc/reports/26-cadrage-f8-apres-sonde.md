# 26 — Cadrage F8 après analyse de frontière

2026-10-24. Étape : consultation terminée, aucune implémentation autorisée par ce rapport.

## Résultats reçus

Pool-9 (auteur B3) et pool-13 (engine-architect) concordent : les observations standard `bad=0,eof=1,fail=1,gcount=0` ne distinguent pas l'erreur OS observée d'un EOF légitime. L'architecte a lu Reader et les principes/ADR ; les résultats exécutés et clauses H_Q/H_A lui ont été fournis par le lead, pas reproduits par lui. HEAD ecb548b fourni, non vérifié indépendamment par le reviewer ; profil engine-architect déclaré dans sa restitution, pas preuve supplémentaire des permissions runtime.

Son premier retour partiel précède la remise des extraits : le retour final les exploite et remplace ce blocage pour l'analyse. Rapport25 absent de son chemin local ; cette limite de transmission reste tracée, elle n'est pas une panne produit.

## Conclusion bornée

- Accepter EOF normal, fichier vide, lecture courte finale et multiple exact4096 reste requis.
- Un rejet inconditionnel sur fail() casserait ces cas ; clear() postlecture n'est pas la cause.
- Un contrôle de taille ne garantit pas identité/stabilité/snapshot, même si deux tailles concordent : mutation/ABA/remplacement restent possibles.
- Aucun canal errno/GetLastError portable garanti autour de ifstream n'a été établi.
- La décision est runtime par chargement de fichier, sans rapport avec une spécialisation compile-time ou des ressources GPU.

**Recommandation retenue dans le mandat actuel : ne pas modifier Reader ; conserver explicitement le gap démontré.** H_A autorisait une calibration, pas un remplacement silencieux de la primitive de lecture. Cela ne ferme ni le défaut ni F8.

Alternatives non adoptées : fread/ferror/feof interne, ou lecture Win32 native. Toutes deux nécessitent une décision préalable sur modes texte/binaire, chemins, erreurs/messages, ownership, fermeture et non-régression ; puis dispositif causal et RED/GREEN adaptés. Aucune nouvelle sonde exécutée ou autorisée ici.

## Preuves et état

Preuve exécutée par lead : [rapport25](25-f8-erreur-os-masquee-en-eof.md), baseline/verrou/recovery et cleanup, source/exécutable/logs archivés. Analyses reçues sans nouvelle exécution. **Catch réel INCONCLUSIVE ; close réellement échoué DEFER/non couvert ; aucune clôture globale F8.** Propriétaire de reprise : coordinateur avec spécialiste build/OS, sous décision séparée.

B4 : patch textuel reçu, application et run final encore à faire. B6 intégré et B1 borné aux trois ordres déjà exécutés (rapports23/24). Aucun gain performance ou PASS global nouveau. Pas de besoin d'arbitrage immédiat tant qu'aucun changement de primitive ni élargissement de campagne n'est entrepris ; toute reprise corrective devra présenter cette décision à l'opérateur.
