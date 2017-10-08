.PHONY: clean All

All:
	@echo "----------Building project:[ 545_4 - Debug ]----------"
	@cd "545_4" && "$(MAKE)" -f  "545_4.mk"
clean:
	@echo "----------Cleaning project:[ 545_4 - Debug ]----------"
	@cd "545_4" && "$(MAKE)" -f  "545_4.mk" clean
