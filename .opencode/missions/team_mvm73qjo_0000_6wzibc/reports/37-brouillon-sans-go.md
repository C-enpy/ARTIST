# 37 — Brouillon sans Go prêt, approbation native requise

2026-10-25. Décision opérateur enregistrée : « Réviser sans Go (Recommandé) ». Préparation de politique uniquement ; aucun worker, build ou test relancé.

## Changement exact

Snapshot conservé dans `model-policy.before-no-go-20261025.json` de cette mission, sans écraser une copie antérieure. Modification de `model-policy.json` : retrait du provider opencode-go et de ses13 entrées modèle/variante, sans autre changement sémantique.

Providers restants : **openai, zai-coding-plan, opencode** ;14 entrées modèle/variante restantes, même ordre relatif et mêmes métriques. Ni nouveau modèle ni score corrigé artificiellement. Version2, startingIntelligence50, classificationBeta0.35, QA proportionate et toutes les clauses H_A/F8 inchangés. Le provider opencode n'est pas opencode-go et n'a pas été retiré par ce choix.

Contrôle PowerShell ciblé de configuration : JSON parsable ; comparaison structurée du brouillon au snapshot filtré uniquement sur Go, égalité complète des autres champs. Aucun traitement de logs ni campagne sans Context Mode. SHA256 fichier courant : **762ad6837788b24cc3b34af3721ba310e5852b4ba378798bc9d6bf7f1fc841c7**. Ce hash de fichier n'est pas présenté comme un hash d'approbation natif.

## État runtime et action utilisateur

Contrôle natif : **Mission policy blocked — changed after approval; run config then start**. Ce verrou est attendu ; le choix en prose autorise le brouillon, pas son exécution.

L'utilisateur doit maintenant lancer :
```text
/ensemble-mission config
```
Puis, après lecture de la présentation native résultante et s'il l'approuve :
```text
/ensemble-mission start
```
Le lead n'invoque pas ces commandes à sa place. Les tâches déjà admises gardent leur ancienne révision ; la reprise QA devra respecter une admission sous la révision nouvellement approuvée, pas un changement de modèle forcé sur pool-15. Aucun shutdown/réadmission répété ou faux completed pour débloquer le board.

## Limites maintenues

Candidat source318bf7a inchangé ; preuves normales/revue/préflight ASan conservées. Restitution QA et prise en compte runtime des profils pas encore vérifiées ; ASan final NOT_RUN. Context Mode toujours non exposé dans le catalogue après redémarrage : pas d'analyse volumineuse avant rétablissement ou accord explicite. Mission non terminée, F8/C2 ouverts et B1 historique non fermé.
